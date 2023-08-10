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

#ifndef NO_LOG
  logConf_t logConf = {.lvl = DBG, .errQuit = false};
  if (init_logger(&logConf) == 0) {
    printf("main pid:%d\n", getpid());
    pid_t log_pid = fork();
    if (log_pid == 0) {
      printf("logger pid:%d\n", getpid());
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
  logConf_t logConf = {.lvl = LNONE, .fn = NULL, .errQuit = false};
  init_logger(&logConf);
#endif

  _DBG("hello %s %s\n", "mario", GIT_COMMIT);
  _INFO("simpleHTTP-%s HTTP/1.1\n", GIT_COMMIT);
  _INFO("http://127.0.0.1:%s/\n", (argc != 2) ? "8080" : argv[1]);
  createSocket(argc, argv);
  _INFO("Listening on port %d\n", server_port);
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
      close(client_sockfd);
      exit(0);
    } else if (pid == -1) {
      perror("handler,fork");
    }
    wait(NULL);
  }
  return 0;
}
