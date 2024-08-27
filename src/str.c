#include "str.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

char* msprintf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int size = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    if (size == -1) return NULL;

    char* str = malloc((size + 1) * sizeof(char));
    if (!str) return NULL;

    va_start(args, fmt);
    size = vsnprintf(str, size + 1, fmt, args);
    va_end(args);
    if (size == -1)
    {
        free(str);
        return NULL;
    }
    return str;
}