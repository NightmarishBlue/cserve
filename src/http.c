#include "http.h"
#include "main.h"
#include "opts.h"
#include "fileio.h"

#include <stdio.h>
#include <stdarg.h>

#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

const char* mthdstrs[] = { "GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE", "PATCH"};
const size_t mthdc = size(mthdstrs);
enum method methodfromstr(const char* mthdstr)
{
    for (int i = 0; i < mthdc; i++)
    {
        if (strncmp(mthdstr, mthdstrs[i], 8) == 0) // biggest is CONNECT
            return (enum method) i;
    }
    return -1;
}

const char* strfrommethod(enum method mthd)
{
    if (mthd < (enum method) 0 || mthd > PATCH) return NULL;
    return mthdstrs[mthd];
}

const struct status statuses[] = {
    {"OK", OK},
    {"Bad Request", BAD_REQUEST},
    {"Not Found", NOT_FOUND},
    {"Length Required", LENGTH_REQUIRED},
    // things for our implementation limits
    {"Payload Too Large", PAYLOAD_TOO_LARGE},
    {"URI Too Long", URI_TOO_LONG},
    {"I'm a teapot", IM_A_TEAPOT}, // things we don't support
    {"Internal Server Error", INTERNAL_SERVER_ERROR},
    {"Not Implemented", NOT_IMPLEMENTED},
};
const size_t statusc = size(statuses);
const struct status* statusfromcode(enum code code)
{
    for (int i = 0; i < statusc; i++)
    {
        const struct status* curr = &statuses[i];
        if (curr->code == code)
            return curr;
    }
    return NULL;
}


const char* verstrs[] = { "1.0", "1.1" };
const size_t verc = size(verstrs);
// from a string like HTTP/X.X, deduce version
enum version versionfromstr(const char verstr[MAX_VERSION_LEN])
{
    if (strncmp(verstr, "HTTP/", 5) != 0)
        return -1;
    verstr += 5; // shift up to the number
    for (int i = 0; i < verc; i++)
    {
        if (strncmp(verstr, verstrs[i], 4) == 0) // version string can be no larger than 4 chars
            return (enum version) i;
    }
    return -1;
}

const char* strfromversion(enum version ver)
{
    if (ver < (enum version) 0 || ver > V1_1) return NULL;
    return verstrs[ver];
}

// callback for readbufcon - returns false when buf ends in str
bool stopafterstring(struct buffer* buf, void* _str)
{
    const char* str = (const char*) _str;
    size_t slen = strnlen(str, BUFF_SIZE); // can't go further than buff size anyway so dont do it
    if (buf->i < slen) return true; // can't compare if there aren't even enough chars
    return strncmp(&buf->data[buf->i - slen], str, slen) != 0;
}

bool stopafterchar(struct buffer* buf, void* _c)
{
    char c = *(char*) _c;
    if (buf->i == 0) return true; // if there is no char, continue
    return buf->data[buf->i - 1] == c;
}

bool stopafterline(struct buffer* buf, void*_)
{
    if (buf->i < 2) return true;
    return strncmp(&buf->data[buf->i - 2], "\r\n", 2) != 0;
}

bool stopafterspace(struct buffer* buf, void*_)
{
    if (buf->i == 0) return true; // if there is no char, continue
    return buf->data[buf->i - 1] != ' '; // continue if the last char isn't space
}

bool sendstatus(fd sock, enum version version, enum code code)
{
    const struct status* status = statusfromcode(code);
    const char* verstr = strfromversion(version);
    bool success = status && verstr;
    if (!verstr)
        verstr = strfromversion(DEFAULT_HTTP_VERSION);
    if (!status)
        status = statusfromcode(INTERNAL_SERVER_ERROR);
    if ((success = status && verstr))
        success = sockprintf(sock, "HTTP/%s %d %s\r\n", verstr, (int) status->code, status->desc) > 0;
    return success; // this is a lot of error checking I know
}

// read the first line out of a socket and figure out if it's a valid request
// return the code to respond with
enum code parsereq(fd sock, struct request* request)
{
    // read first few bytes - ensure they are GET /
    char mthdstr[MAX_METHOD_LEN];
    enum method mthd; // temporarily store the method
    // reject invalid input - smallest method name is GET, 3 chars
    if (readuntilchar(sock, MAX_METHOD_LEN, mthdstr, ' ') < 3 || (mthd = methodfromstr(mthdstr)) == -1)
        return BAD_REQUEST;
    else if (mthd > HEAD) // we only have the first 2 done @u@
    {
        fprintf(stderr, "oops... we don't have '%s' @u@\n", strfrommethod(request->method));
        return NOT_IMPLEMENTED;
    }
    request->method = mthd;

    // if the next character isn't a /, KILL
    if (sgetc(sock, MSG_PEEK) != '/')
        return BAD_REQUEST;

    // extract the path
    // read up to ' ', up to limit
    if (readuntilchar(sock, MAX_REQ_PATH, request->identifier, ' ') == 256)
        return URI_TOO_LONG;

    // extract the version string
    char verstr[MAX_VERSION_LEN];
    readuntilchar(sock, MAX_VERSION_LEN, verstr, '\r');
    enum version version = versionfromstr(verstr);
    if (version == -1)
        return BAD_REQUEST;
    request->version = version;
    
    // if the next character isn't a \n, KILL
    if (sgetc(sock, MSG_PEEK) != '\n')
        return BAD_REQUEST;

    return OK;
}

void serve(fd sock)
{
    struct request req = { .method = GET, .version = DEFAULT_HTTP_VERSION, .identifier = "" };
    enum code rescd = parsereq(sock, &req);

    // TODO parse headers somewhere about here
    // also check if the final line is empty
    printf("%s %s HTTP/%s\n", strfrommethod(req.method), req.identifier, strfromversion(req.version));
    if (rescd != OK) // send the error code
    {
        const struct status* stat = statusfromcode(rescd);
        if (stat)
            fprintf(stderr, "erroneous request: %s\n", stat->desc);
        sendstatus(sock, req.version, rescd);
        return; // TODO break the connection here
    }

    fd file = -1; // file to serve
    off_t fsize = -1; // size of said file (mandated for Content-Length)
    if (req.method == GET)
    {
        bool index = req.identifier[strnlen(req.identifier, 256) - 1] == '/';
        int size;
        if (index)
            size = snprintf(NULL, 0, "%s/%s%s", options->servedir, req.identifier, "index.html");
        else
            size = snprintf(NULL, 0, "%s/%s", options->servedir, req.identifier);

        if (size < 0)
        {
            fprintf(stderr, "snprintf failure\n");
            rescd = INTERNAL_SERVER_ERROR;
            goto respond;
        }

        char filename[size + 1];
        if (index)
            size = snprintf(filename, size + 1, "%s/%s%s", options->servedir, req.identifier, "index.html");
        else
            size = snprintf(filename, size + 1, "%s/%s", options->servedir, req.identifier);

        if (size < 0)
        {
            fprintf(stderr, "snprintf failure\n");
            rescd = INTERNAL_SERVER_ERROR;
            goto respond;
        }

        file = open(filename, O_RDONLY);
        if (file == -1)
        {
            eprintf("could not open file '%s'", filename);
            switch (errno)
            {
                case ENOENT:
                    rescd = NOT_FOUND; break;
                // case EACCES:
                //     rescd = FORBIDDEN; break;
                case ENAMETOOLONG:
                    rescd = URI_TOO_LONG; break;
                default:
                    rescd = INTERNAL_SERVER_ERROR; break;
            }
        }
        else
        {
            fsize = filesize(file);
            if (fsize == -1)
                rescd = INTERNAL_SERVER_ERROR;
        }
    }

respond:
    sendstatus(sock, req.version, rescd);

    if (req.method == GET && fsize > 0) // no need to check file, fsize tells us if it's open
    {
        sockprintf(sock, "Content-Length: %ld\r\n", fsize);
        send(sock, "\r\n", 2, 0);
        transmitfile(sock, file, fsize); // TODO check this return value and maybe break connection if we can't send more
    }
    else
    {
        send(sock, "\r\n", 2, 0);
    }

    if (file != -1)
        close(file);
}