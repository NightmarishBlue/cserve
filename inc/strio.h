// i/o on sockets and strings with the use of helper buffers
#ifndef _SIO_H
#define _SIO_H

#include "sock.h"

#include <stdio.h>

#include <unistd.h>

// get one char from a socket
// returns 0 if the socket is closed
// returns -1 on error
int sgetc(fd sock, int flags);

// simpler function to read at most len - 1 bytes into a buffer of len size until the illegal char appears
// returns the number of chars read, and terminates the string
// returns len on failure, like snprintf
size_t readuntilchar(fd sock, size_t len, char buf[len], char illegal);
#endif