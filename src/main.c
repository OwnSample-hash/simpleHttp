// simpleHttp HTTP/1.1

#include "log/log.h"
#include "lua/lua_.h"
#include "lua/setup.h"
#include "quit_handler.h"
#include "socket.h"
#include "threadJob.h"
#include <lua.h>
#include <poll.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef GIT_COMMIT
#define GIT_COMMIT "none-given"
#endif

driver_t *drv;

int main(int argc, char **argv) {
  log_info("%s %s HTTP/1.1, with %s, with %d args", argv[0], GIT_COMMIT,
           LUA_RELEASE, argc);
  signal(SIGINT, ctrl_c_h);
  drv = calloc(1, sizeof(driver_t));
  drv->socket = calloc(MAX_OPEN_SOCKETS, sizeof(new_sock_t));

#ifndef NO_LOG
  log_set_level(LOG_TRACE);
#else
#warning "No LOG"
  log_set_quiet(1);
#endif

  init("setup.lua", drv);
  if (!drv) {
    log_error("Idk");
    return 1;
  }

  int fds[MAX_OPEN_SOCKETS];
  for (int i = 0; i < drv->socket_count; i++)
    fds[i] = createSocket(drv->socket[i]);
  log_info("Routes root:\"%s\"", drv->routes_root);
  log_info("Server root:\"%s\"", drv->server_root);
  lua_init(drv->routes_root, drv->server_root);
  for (int i = 0; i < drv->socket_count; i++)
    log_info(
        "Url: %s://%s%s%s:%d/", prototoa(drv->socket[i]->proto),
        drv->socket[i]->domain == AF_INET6 ? "[" : "", drv->socket[i]->addr,
        drv->socket[i]->domain == AF_INET6 ? "]" : "", drv->socket[i]->port);

  // INFO: Main loop

  struct pollfd pfds[open_sockets_len];
  for (int i = 0; i < open_sockets_len; i++) {
    pfds[i].fd = open_sockets[i].fd;
    pfds[i].events = POLL_PRI | POLLRDBAND;
  }
  while (1) {
    int ret = poll(pfds, open_sockets_len, -1);
    if (ret > 0) {
      for (int i = 0; i < open_sockets_len; i++) {
        if (pfds[i].revents & POLL_HUP) {
          log_error("Socket %d has hungup", pfds[i].fd);
          exit(1);
        }
        if (pfds[i].revents & POLL_PRI || pfds[i].revents & POLLRDBAND) {
          int port = 0;
          char ipBuffer[INET6_ADDRSTRLEN] = {0};
          int domain = 0;
          for (int j = 0; j < open_sockets_len; j++) {
            domain = open_sockets[j].fd == pfds[i].fd
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
            log_fatal("Mem error");
            exit(1);
          }
          int client_sockfd = accept(pfds[i].fd, client_address, &client_len);
          getAddressAndPort(client_address, ipBuffer, sizeof(ipBuffer), &port);
          log_info("Client %s:%d via fd: %d", ipBuffer, port, client_sockfd);
          free(client_address);
          pid_t pid = fork();
          if (pid == 0) {
            for (int i = 0; i < open_sockets_len; i++) {
              if (close(open_sockets[i].fd)) {
                log_warn("Cloudn't close listening socket %d it's closed tho",
                         open_sockets[i].fd);
                perror("close,handler,server_sock");
              }
            }
            threadJob(client_sockfd, drv->server_root, &(drv->keep_alive));
          } else if (pid == -1) {
            perror("handler,fork");
          } else {
            log_debug("serve_pid:%d", pid);
            if (close(client_sockfd)) {
              log_error("Failed to close %d", client_sockfd);
              perror("close,parent");
            }
          }
          wait(NULL);
        }
      }
    } else {
      log_fatal("Poll failed");
      perror("poll");
    }
  }
  return 0;
}
// Vim: set expandtab tabstop=2 shiftwidth=2:
