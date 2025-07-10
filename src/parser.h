/**
 * @file
 * @brief HTTP request parser
 */

#ifndef __PARSER_NEW_HTTP__
#define __PARSER_NEW_HTTP__

#include "lua/setup.h"

/**
 * @brief Http method enum
 */
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

/**
 * @brief Http parser status
 */
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

/**
 * @typedef request
 * @brief Opaque struct for a request
 *
 */
typedef struct request request_t;
/**<@copydoc request*/

/**
 * @struct pair_ss
 * @brief A string, string pair
 *
 * @var pair_ss::v1
 * The first value of the pair
 *
 * @var pair_ss::v2
 * The second value of the pair
 */
typedef struct pair_ss {
  const char *v1;
  const char *v2;
} pair_ss_t;
/**<@copydoc pair_ss*/

/**
 * @brief Initialize a http request for a fd
 *
 * @param cfd The client file descriptor
 * @return A struct representing the \ref request
 */
request_t *init_request(int cfd);

/**
 * @brief Frees and cleans up the request
 *
 * @param req The request struct
 */
void free_request(request_t *req);

/**
 * @brief Parse the request
 *
 * @param req The request struct
 * @return
 */
parse_status_t parse_request(request_t *req);

/**
 * @brief Processes and respond the request
 *
 * @param req The request struct
 * @param root The root of the server
 * @param keep_alive Keep alive settings
 * @return
 */
parse_status_t process_request(request_t *req, const char *root,
                               const keep_alive_t *keep_alive);

/**
 * @brief Converts a \ref http_method_t "HTTP method" enum to string
 *
 * @param method The method enum to convert to
 * @return The string representing the method
 */
const char *method_to_string(http_method_t method);

#endif
// Vim: set expandtab tabstop=2 shiftwidth=2:
