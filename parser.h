#ifndef __PARSER_SHTTP__
#define __PARSER_SHTTP__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

typedef struct __splits
{
  char *data;
  struct __splits *next;
} *splits_t;

int parseGet (char *payload, size_t spayload, int client_fd);
splits_t splitOn_c (char *str, const char *delimiter);
void erep (int client_fd);
void strncatskip (char *dst, const char *src, size_t count, size_t offset);

#endif /* __PARSER_SHTTP__ */
