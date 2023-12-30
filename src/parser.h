#ifndef __PARSER_SHTTP__
#define __PARSER_SHTTP__

#include <assert.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>

static const char HEADER[] = "HTTP/1.1 200 Ok\r\nConnection: close\r\n";

typedef struct __splits {
  char *data;
  struct __splits *next;
} *splits_t;

typedef enum {
  CALLOC = -1,
  OK_GET = 0,
  NOT_FOUND,
  NO_STAT,
} parseGet_;

int parseReq(char *request, size_t srequest, int client_fd);
parseGet_ parseGet(char *payload, size_t spayload, int client_fd);
splits_t splitOn_c(char *str, const char *delimiter);
void erep(int client_fd);
void strncatskip(char *dst, const char *src, size_t count, size_t offset);

#endif /* __PARSER_SHTTP__ */
