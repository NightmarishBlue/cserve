#include "opts.h"

#include <stdio.h>

#include <unistd.h>
#include <getopt.h>

const char usagemsg[] = "Usage: cserve [-h] <port number>";

// list of all options recognised by cserve
const struct cserveopt optlist[] = { {'h', "Display this help message and exit"} };
const size_t optc = sizeof(optlist) / sizeof(struct cserveopt);

int parseopts(struct cserveconf* opts, int argc, char* argv[])
{
    char valid[optc + 1];
    for (size_t i = 0; i < optc; i++)
    {
        valid[i] = optlist[i].symb;
    }
    valid[optc] = '\0';

    int c;
    while ((c = getopt(argc, argv, valid)) != -1)
    {
        switch (c)
        {
            case 'h':
                opts->help = true;
                break;
        }
    }
    return optind;
}