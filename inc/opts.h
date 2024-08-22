#ifndef _OPTS_H
#define _OPTS_H

#include <stdbool.h>
#include <stdio.h>

// the program's current configuration after parsing options
struct cserveconf
{
    bool help;
};

// the representation of a single option
struct cserveopt
{
    char symb;
    const char* help, *deflt;
};

extern const char* usagemsg;

// parse the input arguments and configure the struct as accordingly
int parseopts(struct cserveconf* opts, int argc, char* argv[]);
// print all registered options in a block to file (mostly for -h)
void printopts(FILE* file);
#endif