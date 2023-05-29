// simpleHttp HTTP/0.9

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int parseGet (char *payload, size_t spayload, int client_fd);
void erep (int client_fd);

int
main (int argc, char **argv)
{
  printf ("simpleHTTP HTTP/0.9\n");
  char buf[BUFSIZ];
  char protoname[] = "tcp";
  struct protoent *protoent;
  int enable = 1, i, newline_found = 0, server_sockfd, client_sockfd;
  socklen_t client_len;
  ssize_t nbytes_read;
  struct sockaddr_in client_address, server_address;
  unsigned short server_port = 8080u;
  if (argc > 1)
    server_port = strtol (argv[1], NULL, 10);
  else
    printf ("Using default port: %d\n", server_port);
  protoent = getprotobyname (protoname);
  if (protoent == NULL)
    {
      perror ("getprotobyname");
      return 1;
    }

  server_sockfd = socket (AF_INET, SOCK_STREAM, protoent->p_proto);
  if (server_sockfd == -1)
    {
      perror ("socket");
      return 1;
    }

  if (setsockopt (server_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable,
                  sizeof (enable))
      < 0)
    {
      perror ("setsockopt(SO_REUSEADDR) failed");
      return 1;
    }

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl (INADDR_ANY);
  server_address.sin_port = htons (server_port);
  if (bind (server_sockfd, (struct sockaddr *)&server_address,
            sizeof (server_address))
      == -1)
    {
      perror ("bind");
      return 1;
    }
  if (listen (server_sockfd, 5) == -1)
    {
      perror ("listen");
      return 1;
    }
  fprintf (stderr, "Listening on port %d\n", server_port);
  while (1)
    {
      client_len = sizeof (client_address);
      client_sockfd = accept (server_sockfd,
                              (struct sockaddr *)&client_address, &client_len);
      while ((nbytes_read = read (client_sockfd, buf, BUFSIZ)) > 0)
        {
          printf ("Recived: ");
          write (STDOUT_FILENO, buf, nbytes_read);
          if (buf[nbytes_read - 1] == '\n')
            newline_found = 1;
          if (parseGet (buf, nbytes_read, client_sockfd) != 0)
            erep (client_sockfd);
          if (newline_found)
            break;
        }
      close (client_sockfd);
    }
  return 0;
}

void
strncatskip (char *dst, const char *src, size_t count, size_t offset)
{
  int i, j;
  for (i = offset, j = strlen (dst); i < count; i++, j++)
    {
      dst[j] = src[i];
    }
  dst[j + 1] = '\0';
}

void
erep (int client_fd)
{
  const char payload[] = "<html>\n"
                         "  404\n"
                         "</html>\n";
  write (client_fd, payload, sizeof (payload));
}

int
parseGet (char *payload, size_t spayload, int client_fd)
{
  char *fn = (char *)calloc (spayload - 3, sizeof (char *));
  strncpy (fn, ".", 1);
  strncatskip (fn, payload, spayload, 4);
  fn[strlen (fn) - 1] = '\0';
  printf ("Opening file: %s\n", fn);
  FILE *fp = fopen (fn, "r");
  if (fp == NULL)
    {
      perror ("fopen");
      return 1;
    }
  char buf[BUFSIZ];
  while (fgets (buf, BUFSIZ, fp) != NULL)
    {
      write (client_fd, buf, strlen (buf));
    }
  if (feof (fp))
    printf ("eof hit on %s\n", fn);
  fclose (fp);
  free (fn);
  return 0;
}
