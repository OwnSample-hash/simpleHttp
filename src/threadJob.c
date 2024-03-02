#include "threadJob.h"
#include "dbg.h"
#include "log/log.h"
#include "lua/virtual_path.h"
#include "socket.h"
#include <netinet/in.h>

void threadJob(int client_sockfd, const char *server) {
  char buf[KB_1 * 8];
  log_info("Serving client fd:%d", client_sockfd);
  int nbytes_read = read(client_sockfd, buf, BUFSIZ);
  int ret;
  switch (parseReq(buf, nbytes_read, client_sockfd, server)) {
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

int serve(int sfd, const driver *drv) {
  while (1) {
    void *client_address = calloc(1, domtosize(drv->socket->domain));
    if (!client_address) {
      log_fatal("Failed to calloc mem to c_addr");
      perror("calloc");
    }
    socklen_t client_len = 0;
    int client_sockfd =
        accept(sfd, (struct sockaddr *)&client_address, &client_len);
    if (client_sockfd == -1) {
      perror("accept");
      return -1;
    }
    char ipBuffer[INET6_ADDRSTRLEN];
    log_trace("client_len=%d, sizeof ipBuffer=%d", client_len,
              sizeof(ipBuffer));
    int port;
    getAddressAndPort(client_address, ipBuffer, sizeof(ipBuffer), &port);
    free(client_address);
    log_info("Client %s:%d via fd: %d", ipBuffer, port, client_sockfd);
    pid_t pid = fork();
    if (pid == 0) {
      threadJob(client_sockfd, drv->server_root);
    } else if (pid == -1) {
      perror("handler,fork");
    } else
      log_debug("serve_pid:%d", pid);
    wait(NULL);
  }
  return 0;
}
