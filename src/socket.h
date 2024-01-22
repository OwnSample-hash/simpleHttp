#ifndef __SOCKET_SHTTP_
#define __SOCKET_SHTTP_
#include "log/log.h"
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

typedef enum { HTTP, HTTPS } protocol;

typedef struct {
  const char *full_key;
} ssl_conf;

#define MAX_ADDR_LEN 12

typedef struct {
  const char *addr;
  const int port;
  const int domain;
  const int listen;
  const protocol proto;
  const ssl_conf *ssl_conf;
} new_sock;

#define MAX_OPEN_SOCKETS 2

typedef struct {
  int fd;
  const new_sock *conf;
} open_socket;

extern open_socket open_sockets[MAX_OPEN_SOCKETS];
extern int open_sockets_len;

int createSocket(const new_sock *sock);
void getAddressAndPort(const struct sockaddr *addr, char *ipBuffer,
                       const size_t ipBufferLength, int *port);

const char *prototoa(const protocol proto);
int *atoproto(const char *asciiz);
unsigned long domtosize(int flag);
#endif /* __SOCKET_SHTTP_ */
