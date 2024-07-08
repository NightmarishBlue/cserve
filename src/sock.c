#include <stdio.h>
#include <stdarg.h>

#include <unistd.h>

#include <sys/socket.h> // socket API
#include <netinet/in.h> // internet socket stuff

typedef int fd;

void eprintf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    perror("");
    va_end(args);
}

void servesock(uint16_t port)
{
    fd sock = socket(AF_INET, SOCK_STREAM, 0); // protocol 0 selects the default for the domain
    if (sock == -1)
        perror("error creating socket: ");

    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET; // the domain we are using
    my_addr.sin_port = htons(port); // the port we will listen on
    my_addr.sin_addr.s_addr = INADDR_ANY; // the addresses we will allow to connect

    struct sockaddr* addr_ptr = (struct sockaddr*) &my_addr;

    if (bind(sock, addr_ptr, sizeof(my_addr)) == -1)
        eprintf("error binding socket %d: ", port);
    
    if (listen(sock, 5) == -1)
        perror("error marking socket passive: ");

    {
        socklen_t size = sizeof(my_addr);
        fd conn = accept(sock, addr_ptr, &size);

        if (conn == -1)
            perror("error accepting connection: ");
        
        if (close(conn) == -1)
            perror("error closing connection socket");
    }

    if (close(sock) == -1)
        perror("error closing socket: ");
}