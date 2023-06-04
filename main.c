// simpleHttp HTTP/1.1

#include "linkList.h"
#include "parser.h"
#include "socket.h"
#include <stdio.h>

int
main (int argc, char **argv)
{
  printf ("simpleHTTP HTTP/1.1\n");
  /*char str[] = "Hello World splited";
  const char delim[] = " ";
  splits_t splits_ = splitOn_c (str, delim);
  displayList (splits_);
  freeList (splits_);*/
  createSocket (argc, argv);
  fprintf (stderr, "Listening on port %d\n", server_port);
  while (1)
    {
      client_len = sizeof (client_address);
      client_sockfd = accept (server_sockfd,
                              (struct sockaddr *)&client_address, &client_len);
      while ((nbytes_read = read (client_sockfd, buf, BUFSIZ)) > 0)
        {
          // printf ("Recived: %s\n", buf);
          // if (buf[nbytes_read - 1] == '\n')
          // newline_found = 1;
          if (parseReq (buf, nbytes_read, client_sockfd) != 0)
            erep (client_sockfd);
          // if (newline_found)
          //  break;
        }
      close (client_sockfd);
    }
  return 0;
}
