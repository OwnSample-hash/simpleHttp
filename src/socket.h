#ifndef __SOCKET_HTTP_
#define __SOCKET_HTTP_
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
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
  char *addr;
  const int port;
  const int domain;
  const int listen;
  const protocol proto;
  const ssl_conf *ssl_conf;
} new_sock_t;

#define MAX_OPEN_SOCKETS 4

typedef struct {
  int fd;
  new_sock_t *conf;
} open_socket;

extern open_socket open_sockets[MAX_OPEN_SOCKETS];
extern int open_sockets_len;

int createSocket(new_sock_t *sock);
void getAddressAndPort(const struct sockaddr *addr, char *ipBuffer,
                       const size_t ipBufferLength, int *port);

const char *prototoa(const protocol proto);
int *atoproto(const char *asciiz);
unsigned long domtosize(int flag);
#endif /* __SOCKET_HTTP_ */
