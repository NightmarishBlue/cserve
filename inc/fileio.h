#ifndef _FILEIO_H
#define _FILEIO_H
#include <stdbool.h>

#include <sys/types.h>

typedef int fd;

// get the size of a file
// return -1 on failure
off_t filesize(fd file);

// check if the given file descriptor refers to a directory
bool isdir(fd file);

// open a directory
// return its file descriptor, or -1 on failure
// will fall back to using fstat if O_DIRECTORY is unavailable
fd opend(const char* path);
#endif