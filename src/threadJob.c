
#include "threadJob.h"

void threadJob(int client_sockfd) {
  char buf[BUFSIZ];
  printf("Serving client, fd %d\n", client_sockfd);
  while ((nbytes_read = read(client_sockfd, buf, BUFSIZ)) > 0) {
    if (parseReq(buf, nbytes_read, client_sockfd) != 0)
      erep(client_sockfd);
    /* break; */
  }
  close(client_sockfd);
  printf("Connection closed on %d exiting\n", client_sockfd);
}
