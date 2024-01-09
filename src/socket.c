#include "socket.h"
#include "log/log.h"
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

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
  if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable))) {
    perror("setsockopt");
    log_fatal("setsockopt faild god man");
    return -1;
  }
  log_trace("Set sockopt SO_REUSEADDR for %d to %d", sfd, enable);

  struct in_addr in_addr = {0};
  int pton = -2;
  if ((pton = inet_pton(sock->domain, sock->addr, &in_addr)) == -1) {
    log_fatal("Failed to convert address (%s)., %d", sock->addr, pton);
    perror("inet_pton");
    close(sfd);
    return -1;
  }
  struct sockaddr_in sockaddr = {.sin_port = htons(sock->port),
                                 .sin_addr = in_addr.s_addr,
                                 .sin_family = sock->domain};
  if (bind(sfd, (const struct sockaddr *)&sockaddr, sizeof(sockaddr))) {
    log_fatal("Failed to bind");
    perror("bind");
    close(sfd);
    return -1;
  }
  log_trace("Binded scoket");

  if (listen(sfd, sock->listen)) {
    log_fatal("Listen failed to set \"%d\"", sock->listen);
    close(sfd);
    perror("listen");
  }
  log_trace("Socket will listen for %d conenction", sock->listen);
  open_sockets[open_sockets_len++] = (open_socket){.fd = sfd, .conf = sock};
  return sfd;
}

void getAddressAndPort(struct sockaddr *addr, char *ipBuffer,
                       size_t ipBufferLength, int *port) {
  if (addr->sa_family == AF_INET) {
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)addr;
    inet_ntop(AF_INET, &(ipv4->sin_addr), ipBuffer, ipBufferLength);
    *port = ntohs(ipv4->sin_port);
  } else {
    fprintf(stderr, "Unsupported address family\n");
    exit(EXIT_FAILURE);
  }
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
    return "unsupported porotcol";
    break;
  }
}
