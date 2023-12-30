#ifndef __SOCKET_SHTTP_
#define __SOCKET_SHTTP_
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

extern char protoname[];
extern struct protoent *protoent;
extern int enable, i, newline_found, server_sockfd, client_sockfd;
extern socklen_t client_len;
extern ssize_t nbytes_read;
extern struct sockaddr_in client_address, server_address;
extern unsigned short server_port;

[[deprecated("Needs a redo incredibly cursed impl")]] int
createSocket(int argc, char **argv);
void getAddressAndPort(struct sockaddr *addr, char *ipBuffer,
                       size_t ipBufferLength, int *port);
#endif /* __SOCKET_SHTTP_ */
