#include "strio.h"

#include <stdarg.h>

int readbuf(fd sock, struct buffer* buf, size_t n)
{
    size_t free = BUFF_SIZE - buf->i;
    int r = read(sock, buf->data + buf->i, (free < n) ? free : n);
    if (r == -1)
    {
        perror("something went wrong reading: ");
        return -1;
    }
        buf->i += r;
    return r;
}

// void readbufcon(fd sock, struct buffer* buf, bool (*callback)(struct buffer*))

int getchbuf(fd sock, struct buffer* buf)
{
    if (buf->i + 1 >= BUFF_SIZE) return -1;
    int r = read(sock, &buf->data[buf->i], 1);
    if (r == 1) buf->i++;
    return r;
}

size_t readbufcon(fd sock, struct buffer* buf, size_t n, bool (*callback)(struct buffer*, void*), void* context)
{
    size_t i = 0;
    while (i++ < n && getchbuf(sock, buf) == 1 && callback(buf, context));
    return i;
}