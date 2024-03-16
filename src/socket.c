#include "socket.h"
#include "log/log.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

unsigned long domtosize(int flag) {
  switch (flag) {
  case AF_INET:
    return sizeof(struct sockaddr_in);
  case AF_INET6:
    return sizeof(struct sockaddr_in6);
  }
  return 0;
}

int createSocket(const new_sock *sock) {
  log_trace("open_sockets_len is %d", open_sockets_len);
  if (open_sockets_len > MAX_OPEN_SOCKETS - 1) {
    log_fatal("Cannot create more socket limit reached");
    return -1;
  }
  int sfd;
  log_debug("Creating socket %d domain, %s:%d address and will listen for %d "
            "connection",
            sock->domain, sock->addr, sock->port, sock->listen);
  if ((sfd = socket(sock->domain, SOCK_STREAM, 0)) == -1) {
    log_fatal("Failed to creat socket");
    perror("socket");
    return -1;
  }
  log_trace("Socket created fd:%d", sfd);

  int enable = 1;
  if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable,
                 sizeof(enable))) {
    perror("setsockopt");
    log_fatal("setsockopt faild god man");
    return -1;
  }
  log_trace("Set sockopt SO_REUSEADDR for %d to %d", sfd, enable);
  void *sockaddr;
  int pton = -2;

  switch (sock->domain) {
  case AF_INET: {
    struct in_addr in_addr = {0};
    if ((pton = inet_pton(sock->domain, sock->addr, &in_addr)) == -1) {
      log_fatal("Failed to convert address (%s)., %d", sock->addr, pton);
      perror("inet_pton");
      close(sfd);
      return -1;
    }
    sockaddr = calloc(1, sizeof(struct sockaddr_in));
    struct sockaddr_in *addr_in = (struct sockaddr_in *)sockaddr;

    addr_in->sin_port = htons(sock->port);
    addr_in->sin_addr = in_addr;
    addr_in->sin_family = sock->domain;
  } break;
  case AF_INET6: {
    struct in6_addr in_addr = {0};
    if ((pton = inet_pton(sock->domain, sock->addr, &in_addr)) == -1) {
      log_fatal("Failed to convert address (%s)., %d", sock->addr, pton);
      perror("inet_pton");
      close(sfd);
      return -1;
    }
    sockaddr = calloc(1, sizeof(struct sockaddr_in6));
    struct sockaddr_in6 *addr_in = (struct sockaddr_in6 *)sockaddr;

    addr_in->sin6_family = sock->domain;
    addr_in->sin6_port = htons(sock->port);
    addr_in->sin6_flowinfo = 0;
    addr_in->sin6_addr = in_addr;
    addr_in->sin6_scope_id = 0;
  } break;
  default:
    log_error("No domain");
    close(sfd);
    return -1;
  }

  if (bind(sfd, sockaddr, domtosize(sock->domain))) {
    log_fatal("Failed to bind");
    perror("bind");
    close(sfd);
    return -1;
  }
  log_trace("Binded scoket");

  free(sockaddr);

  if (listen(sfd, sock->listen)) {
    log_fatal("Listen failed to set \"%d\"", sock->listen);
    close(sfd);
    perror("listen");
  }
  log_trace("Socket will listen for %d conenction", sock->listen);
  open_sockets[open_sockets_len++] = (open_socket){.fd = sfd, .conf = sock};
  return sfd;
}

void getAddressAndPort(const struct sockaddr *addr, char *ipBuffer,
                       const size_t ipBufferLength, int *port) {
  log_trace("family: %i", addr->sa_family);
  if (addr->sa_family == AF_INET) {
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)addr;
    inet_ntop(AF_INET, &(ipv4->sin_addr), ipBuffer, ipBufferLength);
    *port = ntohs(ipv4->sin_port);
  } else if (addr->sa_family == AF_INET6) {
    struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)addr;
    inet_ntop(AF_INET6, &(ipv6->sin6_addr), ipBuffer, ipBufferLength);
    *port = ntohs(ipv6->sin6_port);
  } else {
    log_error("Unsupported address family");
  }
}

#define A2PROTO                                                                \
  X("http", HTTP)                                                              \
  X("https", HTTPS)

int *atoproto(const char *asciiz) {
  int *ret = calloc(1, sizeof(int));
#define X(str, val)                                                            \
  if (strncmp(asciiz, str, strlen(asciiz)) == 0)                               \
    *ret = val;
  A2PROTO
#undef X
  return ret;
}

const char *prototoa(const protocol proto) {
  switch (proto) {
  case HTTP:
    return "http";
    break;
  case HTTPS:
    return "https";
    break;
  default:
    log_trace("%d", proto);
    return "unsupported porotcol";
    break;
  }
}
