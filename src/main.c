#include "opts.h"
#include "sock.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <unistd.h>

int main(int argc, char* argv[])
{
    struct cserveconf opts = { 0 };
    int argindex = parseopts(&opts, argc, argv);

    if (opts.help)
    {
        printf("%s\n", usagemsg);
        printopts(stdout);
        return 0;
    }

    switch (argc - argindex)
    {
        case 0:
            fprintf(stderr, "Error: no port given\n%s\n", usagemsg);
            return 2;
        case 1:
            break;
        default:
            fprintf(stderr, "Warning: ignoring extraneous arguments\n");
    }

    char* portstr = argv[argindex];
    for (size_t i = 0; portstr[i]; i++)
        if (!isdigit((unsigned char) portstr[i]))
        {
            fputs("Error: given port '", stderr); // 19 chars
            i += 19; // we're breaking this loop anyway
            fprintf(stderr, "%s' invalid\n", portstr);
            for (size_t j = 0; j < i; j++) putc(' ', stderr);
            fputs("^\n", stderr);
            return 1;
        }
    
    int portn = atoi(portstr);
    struct sockaddr_in myaddr;

    fd sockid = crtsock();
    if (sockid == -1) return 1;
    if (!(bindport(sockid, &myaddr, portn) && socklisten(sockid)))
    {
        closesock(sockid); // could report this error but 
        return 1;
    }

    while (true)
    {
        struct sockaddr_in clientaddr;
        socklen_t addrsz = sizeof(clientaddr);
        fd clisock = acceptconn(sockid, (struct sockaddr*) &clientaddr, &addrsz);
        if (clisock == -1) continue;
        char name[16];
        if (addrstr(&clientaddr, 16, name))
            printf("Received connection from %s\n", name);
        closesock(clisock);
        break;
    }

    closesock(sockid);
    return 0;
}


void eprintf(const char* format, ...)
{
    // maybe quit if silent
    va_list args;
    va_start(args, format);
    fputs("Error: ", stderr); // would end up writing this 10 times otherwise
    vfprintf(stderr, format, args);
    perror("");
    va_end(args);
}