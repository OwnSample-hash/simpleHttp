// simpleHttp HTTP/1.1

#include "linkList.h"
#include "parser.h"
#include "socket.h"
#include "threadJob.h"
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

int
main (int argc, char **argv)
{
  printf ("simpleHTTP HTTP/1.1\nhttp://tx.rx:8080/\n");
  createSocket (argc, argv);
  fprintf (stderr, "Listening on port %d\n", server_port);
  while (1)
    {
      client_len = sizeof (client_address);
      client_sockfd = accept (server_sockfd,
                              (struct sockaddr *)&client_address, &client_len);
      pid_t pid = fork ();
      switch (pid)
        {
        case -1:
          perror ("fork on client handler\n");
          break;
        case 0:
          threadJob (client_sockfd);
          return 0;
        default:
          printf ("The CHILD pid is %jd\n", (intmax_t)pid);
          break;
        }
    }
  return 0;
}
