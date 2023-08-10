
#include "threadJob.h"

void threadJob(int client_sockfd) {
  char buf[BUFSIZ];
  _INFO("Serving client, fd:%d\n", client_sockfd);
  nbytes_read = read(client_sockfd, buf, BUFSIZ);
  if (parseReq(buf, nbytes_read, client_sockfd) != 0)
    erep(client_sockfd);
  close(client_sockfd);
  _INFO("Connection closed on fd:%d exiting\n", client_sockfd);
}
