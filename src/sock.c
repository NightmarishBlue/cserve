#include "sock.h"
#include "main.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include <unistd.h>

#include <sys/socket.h> // socket API
#include <sys/sendfile.h> // faster, easier way to copy files
#include <sys/time.h>
#include <arpa/inet.h> // get human-readable ip names

fd crtsock()
{
    fd sock = socket(AF_INET, SOCK_STREAM, 0); // protocol 0 selects the default for the domain
    if (sock == -1) eprintf("could not create socket for host ");
    return sock;
}

bool bindport(fd sock, struct sockaddr_in* addr, uint16_t port)
{
    addr->sin_family = AF_INET; // the domain we are using
    addr->sin_port = htons(port); // the port we will listen on
    addr->sin_addr.s_addr = INADDR_ANY; // the addresses we will allow to connect

    bool ret = bind(sock, (struct sockaddr*) addr, sizeof(*addr)) != -1;
    if (!ret)
        eprintf("could not bind socket to port %d", port);
    return ret;
}

bool socklisten(fd sock)
{
    bool ret = listen(sock, QUEUE_LEN) != -1;
    if (!ret)
        eprintf("could not mark socket passive");
    return ret;
}

bool closesock(fd sock)
{
    bool ret = close(sock) == 0;
    if (!ret)
        eprintf("could not close file descriptor");
    return ret;
}

bool closepeer(fd sock)
{
    if (shutdown(sock, SHUT_WR) == -1)
        eprintf("could not shutdown peer connection");
    return closesock(sock);
}

const struct timeval timeout = { 5, 0 }; // 5s timeout
fd acceptconn(fd sock, struct sockaddr* clientaddr, socklen_t* addrlen)
{
    fd conn = accept(sock, clientaddr, addrlen);
    if (conn == -1)
        eprintf("could not create socket for client");
    // if (setsockopt(conn, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) != 0)
    // {
    //     eprintf("could not set timeout on client socket: ");
    //     closesock(conn);
    //     return -1;
    // }
    return conn;
}

bool ipstr(const struct in_addr* iaddr, size_t len, char str[len])
{
    const char* ptr = inet_ntop(AF_INET, iaddr, str, len);
    if (ptr == NULL) // this throws an error if len isn't enough
        eprintf("could not get IP string");
    return ptr != NULL;
}

bool addrstr(const struct sockaddr_in* iaddr, size_t len, char str[])
{
    if (!ipstr(&iaddr->sin_addr, len, str)) return false;
    size_t used = strnlen(str, len), size = len - used;
    uint16_t portnum = ntohs(iaddr->sin_port);
    int written = snprintf(str + used, len - used, ":%u", (unsigned int) portnum);
    return !(written >= size || written < 0); // an error has happened
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


// HACK this function has little reason to exist
// [ ] decide whether to provide a stdc version without dprintf, or just delete it
int sockprintf(fd sock, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int i = vdprintf(sock, fmt, args);
    va_end(args);
    return i;
}

ssize_t transmitfile(fd sock, fd file, size_t n)
{
    ssize_t ret = sendfile(sock, file, NULL, n);
    if (ret == -1)
        eprintf("could not transmit file");
    return -1;
}