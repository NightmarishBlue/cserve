#ifndef _ARGS_H
#define _ARGS_H
#include <stdbool.h>

struct cserveopts
{
    bool help;
};

void parseopts(struct cserveopts* opts, int argc, char* argv[]);
#endif