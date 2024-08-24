#include "fileio.h"
#include "main.h"

#include <sys/stat.h>

off_t filesize(fd file)
{
    struct stat finfo;
    if (fstat(file, &finfo) == -1)
    {
        eprintf("could not stat file");
        return -1;
    }
    return finfo.st_size;
}