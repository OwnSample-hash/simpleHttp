#ifndef __PARSER_SHTTP__
#define __PARSER_SHTTP__

#include "lua/setup.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <strings.h>
#include <unistd.h>

typedef struct __splits {
  char *data;
  struct __splits *next;
} *splits_t;

typedef enum {
  CALLOC = -1,
  OK_GET = 0,
  NOT_FOUND,
  NO_STAT,
  WRONG_PROTOCOL,
  ERR,
} parseGet_t;

parseGet_t parseReq(char *request, size_t srequest, int client_fd,
                    const char *root, const keep_alive_t *keep_alive);

parseGet_t parseGet(char *payload, size_t spayload, int client_fd,
                    const char *root, const keep_alive_t *keep_alive);

/*splits_t splitOn_c(char *str, const char *delimiter);*/

void erep(int client_fd);

/**
 * @brief Concatenate two strings and skip the first `offset` characters
 *
 * @param dst The destination string
 * @param src The source string
 * @param count The number of characters to copy
 * @param offset The number of characters to skip
 */
void strncatskip(char *dst, const char *src, size_t count, size_t offset);

#endif /* __PARSER_SHTTP__ */
