// i/o on sockets and strings with the use of helper buffers
#ifndef _SIO_H
#define _SIO_H

#include "sock.h"

#include <stdio.h>

#include <unistd.h>

#define BUFF_SIZE 512

// is this actually good enough?
// buffers that can be retried could be better.
struct buffer
{
    char data[BUFF_SIZE + 1];
    size_t i; // how many bytes are used
};

// read up to n bytes from sock into buf, advancing its index
// return the number of bytes read or -1 on error
int readbuf(fd sock, struct buffer* buf, size_t n);

// get a single char into the buffer, advancing its index
// return 1 on success, 0 on EOF or -1 on error
int getchbuf(fd sock, struct buffer* buf);

// read n bytes sequentially into buf from sock
// after every byte is read, call the callback on the buffer with context
// stops early if callback returns false
size_t readbufcon(fd sock, struct buffer* buf, size_t n, bool (*callback)(struct buffer*, void*), void* context);
// simpler function to read at most len - 1 bytes into a string until the illegal char appears
// returns the number of chars read, and terminates the string
size_t readuntilchar(fd sock, size_t len, char buf[len], char illegal);
#endif