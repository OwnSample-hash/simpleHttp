// simpleHttp HTTP/1.1

#include "linkList.h"
#include "logger/logger.h"
#include "parser.h"
#include "quit_handler.h"
#include "socket.h"
#include "threadJob.h"
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
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
        perror("dup2");
        return -1;
      }
      handle();
      return 0;
    }
  }
#else
  printf("Not inting logging shit\n");
#endif
  // #define __FILENAME__                                                           \
//   (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define _DBG(...) dbg("[%s:%d] %s", __FILE__, __LINE__, __VA_ARGS__)
  printf("printing\n");
  dbg("%s:%d %s %d\n", __FILE__, __LINE__, "Hello", getpid());
  _DBG("hello, mario%s\n", "asdfasdf");
  info("Test no format\n");
  info("Test format%s\n", "ed!!");
  info("Main proc pid: %d\n", getpid());
  info("simpleHTTP-%s HTTP/1.1\nhttp://127.0.0.1:%s/\n", GIT_COMMIT,
       (argc != 2) ? "8080" : argv[1]);
  return 0;
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
    switch (pid) {
    case -1:
      perror("fork on client handler\n");
      break;
    case 0:
      threadJob(client_sockfd);
      return 0;
    default:
      break;
    }
  }
  return 0;
}
