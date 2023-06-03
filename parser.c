#include "parser.h"
#include "linkList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
erep (int client_fd)
{
  static const char payload[] = "HTTP/1.1 404 Not Found\n"
                                "\n"
                                "<html>\n"
                                "<head>\n"
                                "</head>\n"
                                "<body>\n"
                                "  <h1>404</h1>\n"
                                "</body>\n"
                                "</html>";
  write (client_fd, payload, sizeof (payload));
}

void
freeArray (size_t count, void **array)
{
  for (int i = 0; i < count; i++)
    {
      free (array[i]);
    }
}

splits_t
splitOn_c (char *str, const char *delimiter)
{
  splits_t head = NULL;
  char *token = strtok (str, delimiter);

  while (token != NULL)
    {
      insertNode (&head, token);
      token = strtok (NULL, delimiter);
    }
  return head;
}

int
parseGet (char *payload, size_t spayload, int client_fd)
{
  char *fn = calloc (spayload - 3, sizeof (char));
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
