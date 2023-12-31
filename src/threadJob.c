#include "threadJob.h"
#include "socket.h"

void threadJob(int client_sockfd) {
  char buf[KB_1 * 8];
  log_info("Serving client fd:%d", client_sockfd);
  int nbytes_read = read(client_sockfd, buf, BUFSIZ);
  if (parseReq(buf, nbytes_read, client_sockfd) != 0)
    erep(client_sockfd);
  close(client_sockfd);
  log_info("Connection closed on fd:%d exiting", client_sockfd);
  exit(0);
}

int serve(int sfd) {
  while (1) {
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);
    int client_sockfd =
        accept(sfd, (struct sockaddr *)&client_address, &client_len);
    if (client_sockfd == -1) {
      perror("client accept");
    }
    char ipBuffer[INET_ADDRSTRLEN];
    int port;
    getAddressAndPort((struct sockaddr *)&client_address, ipBuffer,
                      INET_ADDRSTRLEN, &port);
    log_info("Client %s:%d A.K.A %d fd", ipBuffer, port, client_sockfd);
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
      log_debug("serve_pid:%d", pid);
    wait(NULL);
  }
  return 0;
}
