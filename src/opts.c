#include "opts.h"

#include <stdio.h>

#include <unistd.h>
#include <getopt.h>

const char usagemsg[] = "Usage: cserve [-h] <port number>";

// list of all options recognised by cserve
// verbose, version, quiet, max connections, etc.
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

void printopts(FILE* file)
{
    for (size_t i = 0; i < optc; i++)
    {
        for (int j = 0; j < 2; j++) putc(' ', file);
        putc('-', file);
        putc(optlist[i].symb, file);
        for (int j = 0; j < 4; j++) putc(' ', file);
        fputs(optlist[i].help, file);
        putc('\n', file);
    }
}