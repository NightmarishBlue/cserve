#include "opts.h"
#include "sock.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char* argv[])
{
    struct cserveconf opts = { 0 };
    int i = parseopts(&opts, argc, argv);

    if (opts.help)
    {
        printf("%s\n", usagemsg);
        printopts(stdout);
        return 0;
    }

    if (argc - i != 1)
    {
        fprintf(stderr, "Error: no port given\n%s\n", usagemsg);
        return 2;
    }

    char* portstr = argv[i];
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
    servesock(portn);
    return 0;
}