#include "threadJob.h"
#include "log/log.h"
#include "lua/virtual_path.h"
#include "socket.h"

void threadJob(int client_sockfd) {
  char buf[KB_1 * 8];
  log_info("Serving client fd:%d", client_sockfd);
  int nbytes_read = read(client_sockfd, buf, BUFSIZ);
  int ret;
  switch (parseReq(buf, nbytes_read, client_sockfd)) {
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
    log_info("Client %s:%d on fd: %d", ipBuffer, port, client_sockfd);
    pid_t pid = fork();
    if (pid == 0) {
      threadJob(client_sockfd);
    } else if (pid == -1) {
      perror("handler,fork");
    } else
      log_debug("serve_pid:%d", pid);
    wait(NULL);
  }
  return 0;
}
