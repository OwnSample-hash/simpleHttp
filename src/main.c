// simpleHttp HTTP/1.1

#include "linkList.h"
#include "logger/dbg.h"
#include "logger/logger.h"
#include "parser.h"
#include "quit_handler.h"
#include "socket.h"
#include "threadJob.h"
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef GIT_COMMIT
#define GIT_COMMIT "none-given"
#endif

int main(int argc, char **argv) {
  signal(SIGINT, ctrl_c_h);

#ifndef GDB_DEBUG
  if (init_logger(DBG, NULL) == 0) {
    pid_t log_pid = fork();
    if (log_pid == 0) {
      if (dup2(STDOUT_FILENO, 1) == -1) {
        perror("log, dup2");
        return -1;
      }
      handle();
      return 0;
    } else if (log_pid == -1) {
      perror("log,fork");
    }
  }
#else
  printf("Not inting logging shit\n");
#endif
  _DBG("hello %s %s\n", "mario", GIT_COMMIT);
  _INFO("simpleHTTP-%s HTTP/1.1\thttp://127.0.0.1:%s/\n", GIT_COMMIT,
        (argc != 2) ? "8080" : argv[1]);
  _WARN("warn, %zu\n", getpid());
  _ERR("llll%d\n", 234);
  //   return 0;
  createSocket(argc, argv);
  info("Listening on port %d\n", server_port);
  while (1) {
    client_len = sizeof(client_address);
    client_sockfd =
        accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
    if (client_sockfd == -1) {
      perror("client accept");
    }
    pid_t pid = fork();
    if (pid == 0) {
      if (dup2(STDOUT_FILENO, 1) == -1) {
        perror("dup2");
        return -1;
      }
      threadJob(client_sockfd);
      exit(0);
    } else if (pid == -1) {
      perror("handler,fork");
    }
    wait(NULL);
  }
  return 0;
}
