#include "threadJob.h"

void threadJob(int client_sockfd) {
  char buf[KB_1 * 8];
  _INFO("Serving client, fd:%d\n", client_sockfd);
  nbytes_read = read(client_sockfd, buf, BUFSIZ);
  if (parseReq(buf, nbytes_read, client_sockfd) != 0)
    erep(client_sockfd);
  close(client_sockfd);
  _INFO("Connection closed on fd:%d exiting\n", client_sockfd);
  exit(0);
}

int serve() {
  while (1) {
    client_len = sizeof(client_address);
    client_sockfd =
        accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
    if (client_sockfd == -1) {
      perror("client accept");
    }
    pid_t pid = fork();
    if (pid == 0) {
      // if (dup2(STDOUT_FILENO, 1) == -1) {
      //   perror("dup2");
      //   return -1;
      // }
      threadJob(client_sockfd);
    } else if (pid == -1) {
      perror("handler,fork");
    } else
      _DBG("serve_pid:%d\n", pid);
    wait(NULL);
  }
  return 0;
}
