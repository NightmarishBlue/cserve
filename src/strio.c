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

// get a single char into the string, with size_t i tracking its current length
// returns 1 on success, 0 on EOF, or -1 on error
// int getchstr(fd sock, char* str, size_t* i)
// {
//     // if (buf->i + 1 >= BUFF_SIZE) return -1;
//     int r = recv(sock, str + *i, 1, 0);
//     if (r == 1) *i++;
//     return r;
// }

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

// get one char from a socket
// returns 0 if the socket is closed
// returns -1 on error
int sgetc(fd sock, int flags)
{
    unsigned char c[1];
    int i = recv(sock, c, 1, flags);
    if (i != 1) return i;
    return *c;
}

size_t readuntilchar(fd sock, size_t len, char buf[len], char illegal)
{
    if (len == 0) return 0; // safer to simply exit
    // go up len - 1 times
    for (size_t i = 0; i < len; i++)
    {
        int c = sgetc(sock, 0);
        if (c <= 0 || c == illegal)
        {
            buf[i] = '\0';
            return i;
        }
        buf[i] = c;
    }
    buf[--len] = '\0'; // terminate the string
    return len;
}