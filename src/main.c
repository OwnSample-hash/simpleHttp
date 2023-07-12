// simpleHttp HTTP/1.1

#include "linkList.h"
#include "logger/logger.h"
#include "parser.h"
#include "quit_handler.h"
#include "socket.h"
#include "threadJob.h"
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#ifndef GIT_COMMIT
#define GIT_COMMIT "none-given"
#endif

int main(int argc, char **argv) {
  signal(SIGINT, ctrl_c_h);

  init_logger(WARN);
  pid_t log_pid = fork();
  if (log_pid == 0) {
    handle();
    return 0;
  }

  printf("Main proc pid: %d\n", getpid());
  printf("simpleHTTP-%s HTTP/1.1\nhttp://tx.rx:%s/\n", GIT_COMMIT,
         (argc != 2) ? "8080" : argv[1]);
  createSocket(argc, argv);
  fprintf(stderr, "Listening on port %d\n", server_port);
  int counter = 0;
  while (1) {
    client_len = sizeof(client_address);
    client_sockfd =
        accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
    counter++;
    if (counter > 50) {
      fprintf(stderr, "Exiting due to counter reached 50\n");
      break;
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
      printf("The CHILD pid is %jd\n", (intmax_t)pid);
      break;
    }
  }
  return 0;
}
