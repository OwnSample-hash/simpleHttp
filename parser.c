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
parseReq (char *request, size_t srequest, int client_fd)
{
  splits_t lines = splitOn_c (request, (const char *)"\n");
  // lines[0] == GET / HTTP/1.1
  // TODO:
  //  resolve / => /index.html
  //  remove HTTP/1.1 from the end
  char *cpyData = lines->data;
  splits_t wordsGet = splitOn_c (cpyData, (const char *)" ");
  // wordsGet[0] == GET
  // wordsGet[1] == $PATH
  // wordsGet[2] == HTTP/1.1
  if (wordsGet->next != NULL)
    {
      if (strcmp (wordsGet->next->data, "/") == 0)
        wordsGet->next->data = "/index.html";
      else
        return 1;
    }
  else
    {
      return 1;
    }
  char *payload;
  sprintf (payload, "%s %s", wordsGet->data, wordsGet->next->data);
  int ret = parseGet (payload, strlen (payload), client_fd);
  freeList (lines);
  freeList (wordsGet);
  return ret;
}

int
parseGet (char *payload, size_t spayload, int client_fd)
{
  char *fn = calloc (spayload - 3, sizeof (char));
  strncpy (fn, ".", 1);
  strncatskip (fn, payload, spayload, 4);
  // fn[strlen (fn) - 1] = '\0';
  printf ("Opening file: %s\n", fn);
  FILE *fp = fopen (fn, "r");
  if (fp == NULL)
    {
      perror ("fopen");
      return 1;
    }
  const char HEADER[] = "HTTP/1.1 200 Ok\r\n\n";
  write (client_fd, HEADER, strlen (HEADER));
  char buf[BUFSIZ];
  while (fgets (buf, BUFSIZ, fp) != NULL)
    {
      write (client_fd, buf, strlen (buf));
    }
  if (feof (fp))
    fprintf (stderr, "eof hit on %s\n", fn);
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
