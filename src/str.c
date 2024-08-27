#include "str.h"

#include <stdlib.h>
#include <stdarg.h>

char* msprintf(const char* fmt, ...)
{
    char* str;
    va_list args;
    va_start(args, fmt);
    int r = vasprintf(&str, fmt, args);
    va_end(args);
    if (r == -1) return NULL;
    return str;
}