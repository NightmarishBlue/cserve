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

int sgetc(fd sock, int flags)
{
    unsigned char c[1];
    int i = recv(sock, c, 1, flags);
    if (i != 1) return i;
    return *c;
}

size_t readuntilchar(fd sock, const size_t len, char buf[len], char illegal)
{
    if (len == 0) return 0; // safer to simply exit if len is 0
    // go up len - 1 times - i sure hope GCC optimises this
    for (size_t i = 0; i < len - 1; i++)
    {
        int c = sgetc(sock, 0);
        if (c == illegal)
        {
            buf[i] = '\0';
            return i;
        }
        if (c <= 0) // error or socket close
        {
            buf[i] = '\0';
            return len;
        }
        buf[i] = c;
    }
    buf[len - 1] = '\0'; // terminate the string
    // peek at the next char and consume it if it's the breakchar
    if (sgetc(sock, MSG_PEEK) == illegal)
    {
        (void) sgetc(sock, 0);
        return len - 1; // also return the number of bytes consumed
    }
    return len; // return len on bad
}