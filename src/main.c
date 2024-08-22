#include "opts.h"
#include "sock.h"
#include "strio.h"
#include "http.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <unistd.h>

int main(int argc, char* argv[])
{
    struct cserveconf opts = { .help = false, .portn = 80, .servedir = NULL };
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
            fprintf(stderr, "Error: no root directory given\n%s\n", usagemsg);
            return 2;
        case 1:
            break;
        default:
            fprintf(stderr, "Warning: ignoring extraneous arguments\n");
    }
    opts.servedir = argv[argindex]; // TODO stat this and ensure it exists

    struct sockaddr_in myaddr;
    fd sockid = crtsock();
    if (sockid == -1) return 1; // TODO report this error
    if(!(bindport(sockid, &myaddr, opts.portn) && socklisten(sockid)))
    {
        closesock(sockid);
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
        serve(clisock);
        closepeer(clisock);
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