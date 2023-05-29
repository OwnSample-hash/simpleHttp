#include "parser.h"
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

int
splitOn_c (char delimiter, splits_t *_dst, const char *src)
{
  int splits = 1;
  size_t src_len = strlen (src);
  _dst = calloc (1, sizeof (splits_t));
  splits_t *__dst = _dst;
  for (int i = 0; i < src_len; i++)
    {
      if (src[i] == delimiter)
        splits++;
    }
  for (int i = 0; i < splits; i++)
    {
      _dst->next = calloc (1, sizeof (splits_t));
      _dst = _dst->next;
    }
  _dst = __dst;
  int string_len = 0;
  splits = 0;
  for (int i = 0; i < src_len; i++)
    {
      if (src[i] != delimiter)
        {
          string_len++;
        }
      else
        {
          _dst[splits++].ctn = calloc (string_len + 1, sizeof (char));
          string_len = 0;
          _dst = _dst->next;
        }
    }
  _dst = __dst;
  splits = 0;
  string_len = 0;
  for (int i = 0; i < src_len; i++)
    {
      if (src[i] != delimiter)
        {
          _dst->ctn[string_len++] = src[i];
        }
      else
        splits++;
    }
  return splits + 1;
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
