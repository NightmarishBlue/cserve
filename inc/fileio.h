#ifndef _FILEIO_H
#define _FILEIO_H
#include <sys/types.h>

typedef int fd;

// get the size of a file
// return -1 on failure
off_t filesize(fd file);
#endif