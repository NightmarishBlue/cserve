#include "fileio.h"
#include "main.h"

#include <fcntl.h>
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

fd opend(const char* path)
{
    #ifdef O_DIRECTORY
        return open(path, O_RDONLY | O_DIRECTORY);
    #else
        fd dir = open(path, O_RDONLY);
        if (dir == -1) return -1;
        struct stat dinfo;
        if (fstat(dir, &dinfo) != -1 && S_ISDIR(dinfo.st_mode)) return dir;
        return -1;
    #endif
}