#include "threadJob.h"
#include "bytes.h"
#include "log/log.h"
#include "lua/setup.h"
#include "lua/virtual_path.h"
#include "parser.h"
#include "socket.h"
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void thread_quit(int code, void *arg) {
  quit_status_t *quit_status = arg;
  log_info("Done serving client: %d", quit_status->cfd);
  if (quit_status->keep_alive->keep_alive) {
    log_info("Thread could server %d request with to %d",
             quit_status->keep_alive->max, quit_status->keep_alive->timeout);
  } else {
    log_info("Thread type one shot");
  }
  log_info("Quit with code %d", code);
}

void threadJob(int client_sockfd, const char *server,
               const keep_alive_t *keep_alive) {
  signal(SIGINT, NULL);
  quit_status_t quit_status = {.cfd = client_sockfd};
  if (on_exit(thread_quit, &quit_status) != 0) {
    perror("atexit");
    log_fatal("Faild to register at exit handler");
    exit(THREAD_FAIL);
  }
  char buf[KB_1 * 8];
  keep_alive_t local_keep_alive = {};
  memcpy(&local_keep_alive, keep_alive, sizeof(keep_alive_t));
  quit_status.keep_alive = &local_keep_alive;
  log_info("Serving client fd:%d", client_sockfd);
  if (local_keep_alive.keep_alive)
    log_info("All connection will have a %d sec time out with %d max request",
             local_keep_alive.timeout, local_keep_alive.max);
  else
    log_info("All connection will be closed!");
  if (local_keep_alive.keep_alive) {
    while (local_keep_alive.max > 0) {
      log_info("BEGENNING OF REQUEST");
      struct pollfd fds[1];
      fds[0].fd = client_sockfd;
      fds[0].events = POLL_IN;
      int ret = poll(fds, 1, local_keep_alive.timeout * 1000);
      if (ret == 0) {
        log_warn("Client %d timed out with %d sec", client_sockfd,
                 local_keep_alive.timeout);
        close(client_sockfd);
        exit(THREAD_TIMEOUT);
      }
      if (ret < 0) {
        log_error("Poll failed!");
        perror("poll,client,fork");
        close(client_sockfd);
        exit(THREAD_FAIL);
      }
      if (fds[0].revents & POLL_HUP) {
        log_error("Socket %d has hungup", fds[0].fd);
        close(client_sockfd);
        exit(THREAD_SOCKET_HUNGUP);
      }
      if (fds[0].revents & POLL_PRI || fds[0].revents & POLLRDBAND) {
        int nbytes_read = read(client_sockfd, buf, BUFSIZ);
        switch (parseReq(buf, nbytes_read, client_sockfd, server, keep_alive)) {
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
      }
      local_keep_alive.max--;
      log_info("END OF REQUEST");
    }
  } else {
    int nbytes_read = read(client_sockfd, buf, BUFSIZ);
    switch (parseReq(buf, nbytes_read, client_sockfd, server, keep_alive)) {
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
  }
  if (!close(client_sockfd)) {
    perror("close");
    log_debug("Failed to close socket %d", client_sockfd);
  }
  log_info("Connection closed on fd:%d exiting", client_sockfd);
  exit(0);
}

int serve(const driver *drv) {
  struct pollfd fds[open_sockets_len];
  for (int i = 0; i < open_sockets_len; i++) {
    fds[i].fd = open_sockets[i].fd;
    fds[i].events = POLL_PRI | POLLRDBAND;
  }
  while (1) {
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
            log_fatal("Mem error");
            exit(1);
          }
          int client_sockfd = accept(fds[i].fd, client_address, &client_len);
          getAddressAndPort(client_address, ipBuffer, sizeof(ipBuffer), &port);
          log_info("Client %s:%d via fd: %d", ipBuffer, port, client_sockfd);
          free(client_address);
          pid_t pid = fork();
          if (pid == 0) {
            if (!close(fds[i].fd)) {
              log_error("Cloudn't close listening socket %d", fds[i].fd);
              perror("close,handler,server_sock");
            }
            threadJob(client_sockfd, drv->server_root, &(drv->keep_alive));
          } else if (pid == -1) {
            perror("handler,fork");
          } else {
            log_debug("serve_pid:%d", pid);
            if (!close(client_sockfd)) {
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
