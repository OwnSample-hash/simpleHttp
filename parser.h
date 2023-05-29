#ifndef __PARSER_SHTTP__
#define __PARSER_SHTTP__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

typedef struct __splits
{
  char *ctn;
  struct __splits *next;
} splits_t;

int parseGet (char *payload, size_t spayload, int client_fd);
int splitOn_c (char delimiter, splits_t *dst, const char *src);
void freeArray (size_t count, void **array);
void erep (int client_fd);
void strncatskip (char *dst, const char *src, size_t count, size_t offset);

#endif /* __PARSER_SHTTP__ */
