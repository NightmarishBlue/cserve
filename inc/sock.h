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
#endif