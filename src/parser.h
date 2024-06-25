#ifndef __PARSER_SHTTP__
#define __PARSER_SHTTP__

#include "lua/setup.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef BIN_NAME
#define BIN_NAME "simpleHttpd-" GIT_COMMIT
#endif

#ifndef HEADER_BASE
#define HEADER_BASE "HTTP/1.1 200 Ok\r\nServer: " BIN_NAME "\r\n"
#endif

static const char HEADER_CLOSE[] = HEADER_BASE "Connection: close\r\n";
static const char HEADER_KEEP[] = HEADER_BASE "Connection: Keep-Alive\r\nKeep-Alive: "
    "timeout=%d, max=%d\r\n";

typedef struct __splits {
  char *data;
  struct __splits *next;
} *splits_t;

typedef enum {
  CALLOC = -1,
  OK_GET = 0,
  NOT_FOUND,
  NO_STAT,
} parseGet_t;

parseGet_t parseReq(char *request, size_t srequest, int client_fd,
                    const char *root, const keep_alive_t *keep_alive);
parseGet_t parseGet(char *payload, size_t spayload, int client_fd,
                    const char *root, const keep_alive_t *keep_alive);
splits_t splitOn_c(char *str, const char *delimiter);
void erep(int client_fd);
void strncatskip(char *dst, const char *src, size_t count, size_t offset);
int genHeader(char **dst, const keep_alive_t *keep_alive);

#endif /* __PARSER_SHTTP__ */
