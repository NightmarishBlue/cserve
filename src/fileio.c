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

bool isdir(fd file)
{
    struct stat dinfo;
    if (fstat(file, &dinfo) == -1)
        return false; // no way for the caller to tell if this is the reason for false...
    return S_ISDIR(dinfo.st_mode);
}

fd opend(const char* path)
{
    #ifdef O_DIRECTORY
        return open(path, O_RDONLY | O_DIRECTORY);
    #else
        fd dir = open(path, O_RDONLY);
        if (dir == -1) return -1;
        struct stat dinfo;
        if (isdir(dir)) return dir;
        return -1;
    #endif
}

#include <linux/openat2.h>
#include <sys/syscall.h>
#include <unistd.h>

int openat2(fd dir, const char* path, const struct open_how* how, size_t size)
{
    return syscall(SYS_openat2, dir, path, how, size);
}

fd openunder(fd dir, const char* relpath, int flags)
{
    struct open_how how = { .flags = flags, .resolve = RESOLVE_IN_ROOT };
    return openat2(dir, relpath, &how, sizeof(how));
}