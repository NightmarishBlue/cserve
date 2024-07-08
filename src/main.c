#include "opts.h"
#include "sock.h"

#include <stdio.h>

int main(int argc, char* argv[])
{
    struct cserveconf opts = { 0 };
    int i = parseopts(&opts, argc, argv);

    if (opts.help)
    {
        printf(usagemsg);
        return 0;
    }

    if (argc - i != 1)
    {
        fprintf(stderr, "Error: no port given\n%s\n", usagemsg);
        return 2;
    }
    
    return 0;
}