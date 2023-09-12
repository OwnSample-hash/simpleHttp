#include "socket.h"

char protoname[] = "tcp";
struct protoent *protoent;
int enable = 1, i, newline_found = 0, server_sockfd, client_sockfd;
socklen_t client_len;
ssize_t nbytes_read;
struct sockaddr_in client_address, server_address;
unsigned short server_port = 8080u;

int createSocket(int argc, char **argv) {
  if (argc > 1)
    server_port = strtol(argv[1], NULL, 10);
  else
    printf("Using default port: %d\n", server_port);
  protoent = getprotobyname(protoname);
  if (protoent == NULL) {
    perror("getprotobyname");
    return 1;
  }

  server_sockfd = socket(AF_INET, SOCK_STREAM, protoent->p_proto);
  if (server_sockfd == -1) {
    perror("socket");
    return 1;
  }

  if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable,
                 sizeof(enable)) < 0) {
    perror("setsockopt(SO_REUSEADDR) failed");
    return 1;
  }

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(server_port);
  if (bind(server_sockfd, (struct sockaddr *)&server_address,
           sizeof(server_address)) == -1) {
    perror("bind");
    return 1;
  }

  if (listen(server_sockfd, 5) == -1) {
    perror("listen");
    return 1;
  }
  return 0;
}
