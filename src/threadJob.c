#include "threadJob.h"
#include "bytes.h"
#include "log/log.h"
#include "lua/setup.h"
#include "parser.h"
#include "plugin/plugin.h"
#include "plugin/simple_http_plugin.h"
#include "quit_handler.h"
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>

void thread_quit() { ctrl_c_h(-1); }

void threadJob(int client_sockfd, const char *server,
               const keep_alive_t *keep_alive) {
  signal(SIGINT, NULL);
  if (atexit(thread_quit)) {
    perror("atexit");
    log_fatal("Faild to register at exit handler");
    exit(THREAD_FAIL);
  }
  char *buf = calloc(KB_1 * 8, sizeof(char));
  if (buf == NULL) {
    log_fatal("Failed to allocate buffer");
    exit(THREAD_FAIL);
  }
  save_ptr(buf);
  keep_alive_t local_keep_alive = {};
  memcpy(&local_keep_alive, keep_alive, sizeof(keep_alive_t));
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
        request_t *req = init_request(client_sockfd);
        if (req == NULL) {
          log_fatal("Failed to allocate request");
          free(buf);
          exit(THREAD_FAIL);
        }
        parse_request(req);
        process_request(req, server, &local_keep_alive);
        free_request(req);
      }
      local_keep_alive.max--;
      log_info("END OF REQUEST");
    }
  } else {
    trigger_event(g_plugin, event_pre_on_incoming_client, client_sockfd);
    request_t *req = init_request(client_sockfd);
    if (req == NULL) {
      log_fatal("Failed to allocate request");
      free(buf);
      exit(THREAD_FAIL);
    }
    parse_request(req);
    process_request(req, server, &local_keep_alive);
    free_request(req);
    trigger_event(g_plugin, event_post_on_incoming_client, client_sockfd);
  }
  if (close(client_sockfd)) {
    perror("close");
    log_debug("Failed to close socket %d", client_sockfd);
  }
  log_info("Connection closed on fd:%d exiting", client_sockfd);
  exit(0);
}
// Vim: set expandtab tabstop=2 shiftwidth=2:
