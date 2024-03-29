#include "threadJob.h"
#include "log/log.h"
#include "lua/virtual_path.h"
#include "socket.h"
#include <netinet/in.h>
#include <poll.h>

void threadJob(int client_sockfd, const char *server) {
  char buf[KB_1 * 8];
  log_info("Serving client fd:%d", client_sockfd);
  int nbytes_read = read(client_sockfd, buf, BUFSIZ);
  int ret;
  switch (parseReq(buf, nbytes_read, client_sockfd, server)) {
  case NIL:
    log_debug("Nil resp");
    erep(client_sockfd);
    break;
  case HANDLED:
    log_debug("Client handeld other ways");
    break;
  default:
    break;
  }
  close(client_sockfd);
  log_info("Connection closed on fd:%d exiting", client_sockfd);
  exit(0);
}

int serve(int *fds, const driver *drv) {
  while (1) {
    struct pollfd fds[open_sockets_len];
    for (int i = 0; i < open_sockets_len; i++) {
      fds[i].fd = open_sockets[i].fd;
      fds[i].events = POLL_PRI | POLLRDBAND;
    }
    int ret = poll(fds, open_sockets_len, -1);
    if (ret > 0) {
      for (int i = 0; i < open_sockets_len; i++) {
        if (fds[i].revents & POLL_HUP) {
          log_error("Socket %d has hungup", fds[i].fd);
          exit(1);
        }
        if (fds[i].revents & POLL_PRI || fds[i].revents & POLLRDBAND) {
          int port = 0;
          char ipBuffer[INET6_ADDRSTRLEN] = {0};
          int domain = 0;
          for (int j = 0; j < open_sockets_len; j++) {
            domain = open_sockets[j].fd == fds[i].fd
                         ? open_sockets[j].conf->domain
                         : -1;
            if (domain > 0)
              break;
          }
          log_trace("!!!sock->domian=%d", domain);
          socklen_t client_len = domtosize(domain);
          void *client_address = calloc(1, client_len);
          if (!client_address) {
            perror("calloc,accept_logic");
            log_fatal("Mem erroy");
            exit(1);
          }
          int client_sockfd = accept(fds[i].fd, client_address, &client_len);
          getAddressAndPort(client_address, ipBuffer, sizeof(ipBuffer), &port);
          log_info("Client %s:%d via fd: %d", ipBuffer, port, client_sockfd);
          pid_t pid = fork();
          if (pid == 0) {
            threadJob(client_sockfd, drv->server_root);
          } else if (pid == -1) {
            perror("handler,fork");
          } else
            log_debug("serve_pid:%d", pid);
          wait(NULL);
        }
      }
    }
  }
  return 0;
}
