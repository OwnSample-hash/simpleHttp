// simpleHttp HTTP/1.1

#include "linkList.h"
#include "logger/logger.h"
#include "parser.h"
#include "socket.h"
#include "threadJob.h"
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
  init_logger(WARN);
  printf("simpleHTTP HTTP/1.1\nhttp://tx.rx:%s/\n",
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
