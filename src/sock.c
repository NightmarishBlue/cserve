#include "sock.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#include <unistd.h>

#include <sys/socket.h> // socket API

void eprintf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    fputs("Error: ", stderr); // would end up writing this 10 times otherwise
    vfprintf(stderr, format, args);
    perror("");
    va_end(args);
}

fd crtsock()
{
    fd sock = socket(AF_INET, SOCK_STREAM, 0); // protocol 0 selects the default for the domain
    if (sock == -1) eprintf("could not create socket for host: ");
    return sock;
}

bool bindport(fd sock, struct sockaddr_in* addr, uint16_t port)
{
    addr->sin_family = AF_INET; // the domain we are using
    addr->sin_port = htons(port); // the port we will listen on
    addr->sin_addr.s_addr = INADDR_ANY; // the addresses we will allow to connect

    bool ret = bind(sock, (struct sockaddr*) addr, sizeof(*addr)) != -1;
    if (!ret)
        eprintf("could not bind socket to port %d: ", port);
    return ret;
}

bool socklisten(fd sock)
{
    bool ret = listen(sock, QUEUE_LEN) != -1;
    if (!ret)
        eprintf("could not mark socket passive: ");
    return ret;
}

    // {
    //     socklen_t size = sizeof(sock);
    //     fd conn = accept(sock, addr_ptr, &size);

    //     if (conn == -1)
    //         perror("error accepting connection: ");
        
    //     if (close(conn) == -1)
    //         perror("error closing connection socket");
    // }

    // if (close(sock) == -1)
    //     perror("error closing socket: ");