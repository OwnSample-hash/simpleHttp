#ifndef __PARSER_NEW_HTTP__
#define __PARSER_NEW_HTTP__

#include "lua/setup.h"
#include <stdlib.h>

typedef enum {
  HTTP_METHOD_UNKNOWN = -1,
  HTTP_METHOD_GET,
  HTTP_METHOD_POST,
  HTTP_METHOD_PUT,
  HTTP_METHOD_DELETE,
  HTTP_METHOD_PATCH,
  HTTP_METHOD_OPTIONS,
  HTTP_METHOD_HEAD,
} http_method_t;

typedef enum {
  PARSE_OK = 0,
  PARSE_ERR,
  PARSE_ERR_NO_MEMORY,
  PARSE_ERR_INVALID_REQUEST,
  PARSE_ERR_INVALID_METHOD,
  PARSE_ERR_INVALID_URI,
  PARSE_ERR_FILE_NOT_FOUND,
  PARSE_ERR_INVALID_RESPONSE,
  PARSE_ERR_WRITE_FAILED,
} parse_status_t;

typedef struct request request_t;

typedef struct {
  const char *key;
  const char *value;
} pair_ss_t;

request_t *init_request(int cfd);

void free_request(request_t *req);

parse_status_t parse_request(request_t *req);

parse_status_t process_request(request_t *req, const char *root,
                               const keep_alive_t *keep_alive);

const char *method_to_string(http_method_t method);

void get_buffer(const request_t *req, const char **buf, size_t *size);

#endif
// Vim: set expandtab tabstop=2 shiftwidth=2:
