#ifndef _SOCK_H
#define _SOCK_H

#include <stdint.h>
#include <stdbool.h>

#include <netinet/in.h> // internet socket stuff

#define QUEUE_LEN 5 // how many connection requests to keep queued before we deal with them

typedef int fd;

// these functions print diagnostic error messages

// register a socket and return its fd
fd crtsock();
// bind a socket to the given port, storing details in an external address struct
bool bindport(fd sock, struct sockaddr_in* addr, uint16_t port);
// put the given socket into the listening state
bool socklisten(fd sock);
// close a socket (only useful for the error output)
bool closesock(fd sock);
fd acceptconn(fd sock, struct sockaddr* clientaddr, socklen_t* addrlen);
bool ipstr(const struct in_addr* iaddr, size_t len, char str[len]);
// with an internet address, place the string IP:PORT into str
// return true if successful
bool addrstr(const struct sockaddr_in* iaddr, size_t size, char str[]);
#endif