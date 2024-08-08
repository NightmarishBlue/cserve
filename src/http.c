#include "http.h"
#include "main.h"

#include <stdio.h>
#include <stdarg.h>

#include <string.h>

#include <unistd.h>
#include <errno.h>

const char mthdstrs[][MAX_METHOD_LEN] = { "GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE", "PATCH"};
const size_t mthdc = sizeof(mthdstrs) / sizeof(mthdstrs[0]);
enum method methodfromstr(const char* mthdstr)
{
    for (int i = 0; i < mthdc; i++)
    {
        if (strncmp(mthdstr, mthdstrs[i], sizeof(mthdstrs[0]) / sizeof(char)) == 0)
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
  //{"..................... // this is the max size a message can currently be
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

const char verstrs[][4] = { "1.0", "1.1" };
const size_t verc = size(verstrs);
// from a string like HTTP/X.X, deduce version
enum version versionfromstr(const char verstr[9])
{
    if (strncmp(verstr, "HTTP/", 5) != 0)
        return -1;
    verstr += 5; // shift up to the number
    for (int i = 0; i < verc; i++)
    {
        if (strncmp(verstr, verstrs[i], size(verstrs[0])) == 0)
            return (enum version) i;
    }
    return -1;
}

const char* strfromversion(enum version ver)
{
    if (ver < (enum version) 0 || ver > _1_1) return NULL;
    return verstrs[ver]; // HACK maybe do some error checking
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

void sendstatus(fd sock, enum version ver, int code)
{
    for (int i = 0; i < statusc; i++)
    {
        const struct status* curr = &statuses[i];
        if (curr->code == code)
        {
            sockprintf(sock, "HTTP/%s %d %s\r\n", strfromversion(ver), (int) curr->code, curr->desc);
            return;
        }
    }
}

void serve(fd sock)
{
    /*
        read first few bytes - ensure they are GET /
            close on 400 if not
        read up to ' ', up to limit of ~128
            return 414 if a space hasn't been found
            now we can obtain the path
        read up to HTTP/, up to N bytes
            return 400 if not
        check version number
            return 400 if bad
        read up to \r\n - if we got here, this is PROBABLY a good request
    */

    struct request req;

    char mthdstr[MAX_METHOD_LEN]; // longest method name is CONNECT, @7 chars
    // reject invalid input - smallest method name is GET, 3 chars
    if (readuntilchar(sock, MAX_METHOD_LEN, mthdstr, ' ') < 3 || (req.method = methodfromstr(mthdstr)) == -1)
    {
        printf("bad method '%s'\n", mthdstr);
        sendstatus(sock, _1_1, BAD_REQUEST); // HACK dont like how it assumes 1.1
        return;
    }
    else if (req.method > HEAD) // we only have the first 2 done @u@
    {
        fprintf(stderr, "oops... we don't have '%s' @u@\n", strfrommethod(req.method));
        sendstatus(sock, _1_1, NOT_IMPLEMENTED);
        return;
    }

    sendstatus(sock, _1_1, OK);

    // pull out path - read until ' '
    // save current location pointer
    // char* path = &request.data[request.i];
    // size_t pathsz = readbufcon(sock, &request, 128, &stopafterspace, NULL);
    // reject if the last char is not space
    // if (request.data[request.i - 1] != ' ')
    // {
    //     printf("bad path\n");
    //     sendstatus(sock, _1_1, URI_TOO_LONG);
    //     return;
    // }
    // could maybe copy this string somewhere? or put it somewhere else to begin with?

    // read 'HTTP/X.X'
    // char* verstr = &request.data[request.i]; // 8 chars long
    // enum version ver;
    // if (readbuf(sock, &request, 8) != 8 || (ver = versionfromstr(verstr)) == -1)
    // {
    //     printf("bad version\n");
    //     sendstatus(sock, _1_1, BAD_REQUEST);
    //     return;
    // }

    // check that it ends in a \r\n
    // if (readbuf(sock, &request, 2) != 2 || strncmp(&request.data[request.i - 3], "\r\n", 2) != 0)
    // {
    //     printf("bad req\n");
    //     sendstatus(sock, ver, BAD_REQUEST);
    //     return;
    // }

    // in future we can parse headers

    // ok now we can mangle strings :)
    // ok, now we can serve the file, if it's there
    // path[pathsz] = '\0';
    // FILE* f = fopen(path, "r");
    // if (!f)
    // {
    //     eprintf("could not open file '%s'\n", path);
    //     sendstatus(sock, ver, errno == ENOENT ? NOT_FOUND: INTERNAL_SERVER_ERROR);
    //     return;
    // }

    // sendstatus(sock, ver, OK);

    if (req.method == GET)
    {
        // send the file
    }

    // fclose(f);
}