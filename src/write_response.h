#ifndef __WRITE_RESPONSE_H__
#define __WRITE_RESPONSE_H__

#include "bytes.h"
#include "lua/setup.h"
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#define UNUSED(x) (void)(x)

#ifndef BIN_NAME
#define BIN_NAME "simpleHttpd-" GIT_COMMIT
#endif

#ifndef HEADER_BASE
#define HEADER_BASE "HTTP/1.1 %d %s\r\nServer: " BIN_NAME "\r\n"
#endif

#define HTTP_EMPTY_LINE "\r\n"
#define HTTP_EMPTY_LINE_LEN 2

#define DEFAULT_LEN 1 * MB_1

/**
 * @brief The header close format string
 */
static const char HEADER_CLOSE[] = HEADER_BASE "Connection: close\r\n";

/**
 * @brief The header keep alive format string
 */
static const char HEADER_KEEP[] =
    HEADER_BASE "Connection: Keep-Alive\r\nKeep-Alive: "
                "timeout=%d, max=%d\r\n";

/**
 * @typedef pair
 * @brief A pair of integer and string
 *
 */
typedef struct pair {
  /**
   * @brief The integer value
   */
  const int p1;
  /**
   * @brief The string value
   */
  const char *p2;
} pair_is_t;

/**
 * @struct map
 * @brief A map of a pair of integer and string and a string
 *
 */
typedef struct map {
  const pair_is_t key;
  const char *value;
} map_t;

/**
 * @brief Maps the status code to the status message
 */
static const map_t STATUS_MAP[] = {
    {{200, "200"}, "Ok"},
    {{201, "201"}, "Created"},
    {{204, "204"}, "No Content"},
    {{206, "206"}, "Partial Content"},
    {{301, "301"}, "Moved Permanently"},
    {{302, "302"}, "Found"},
    {{304, "304"}, "Not Modified"},
    {{307, "307"}, "Temporary Redirect"},
    {{308, "308"}, "Permanent Redirect"},
    {{400, "400"}, "Bad Request"},
    {{404, "404"}, "Not Found"},
    {{405, "405"}, "Method Not Allowed"},
    {{408, "408"}, "Request Timeout"},
    {{409, "409"}, "Conflict"},
    {{411, "411"}, "Length Required"},
    {{413, "413"}, "Payload Too Large"},
    {{414, "414"}, "URI Too Long"},
    {{415, "415"}, "Unsupported Media Type"},
    {{416, "416"}, "Range Not Satisfiable"},
    {{417, "417"}, "Expectation Failed"},
    {{429, "429"}, "Too Many Requests"},
    {{500, "500"}, "Internal Server Error"},
    {{501, "501"}, "Not Implemented"},
    {{505, "505"}, "HTTP Version Not Supported"},
    {{0, NULL}, NULL},
};

/**
 * @typedef header_t
 * @struct header
 * @brief A header struct
 *
 * @var header::name
 * The header name
 *
 * @var header::value
 * The header value
 */
typedef struct header {
  const char *name;
  const char *value;
} header_t;

/**
 * @typedef response
 * @brief The response opaque struct
 *
 */
typedef struct response response_t;

/**
 * @brief Sends the response
 *
 * @param res The response struct
 * @return int The number of bytes written
 */
size_t write_response(response_t *res);

/**
 * @brief Creates a new response
 *
 * @param cfd The client file descriptor
 * @param status The status code
 * @param keep_alive The keep alive struct
 * @return
 */
response_t *new_response(int cfd, const map_t *status,
                         const keep_alive_t *keep_alive);

/**
 * @brief Adds a header to the response
 *
 * @param res The response struct
 * @param name The header name
 * @param value The header value
 */
void add_header(response_t *res, const char *name, const char *value);

/*header_t *expandHeader(header_t *header, size_t size);*/

/**
 * @brief Trims the header in the response
 *
 * @param resp The response struct
 */
void trim_response(response_t *resp);

/**
 * @brief Frees the response struct
 *
 * @param resp The response struct
 */
void free_response(response_t *resp);

/**
 * @brief Generate the base header
 *
 * @param dst The destination buffer
 * @param status The status code
 * @param keep_alive The keep alive struct
 * @return int The length of the header
 */
int gen_header(char **dst, const map_t *status, const keep_alive_t *keep_alive);

/**
 * @brief Set the payload
 *
 * @param res The response struct
 * @param payload The payload
 * @param len The length of the payload
 * @return size_t The length of the payload
 */
size_t set_payload(response_t *res, const char *payload, size_t len);

/**
 * @brief Adds to the payload
 *
 * @param res The response struct
 * @param payload The payload to add
 * @param len The length of the payload to add
 * @return size_t The length of the payload after adding
 */
size_t add_to_Payload(response_t *res, const char *payload, size_t len);

/**
 * @brief Sets the status code of the response
 *
 * @param res The response struct
 * @param code The status code to set
 */
void set_status(response_t *res, int code);

/**
 * @brief Adds a file to the payload
 * Also sets the content length and type based on the file's MIME type
 *
 * @param res The response struct
 * @param file The file to add to the payload
 * @return size_t The length of the payload after adding the file
 */
size_t add_file_to_payload(response_t *res, const char *file, const char *mode);

/**
 * @brief Checks if the response is valid
 *
 * @param res The response struct
 * @return bool True if the response is valid, false otherwise
 */
bool is_valid(const response_t *res);

/**
 * @brief Lookup the status code
 *
 * @param code The status code
 * @return const map_t* The status code
 */
const map_t *lookup_status(int code);
#endif // __WRITE_RESPONSE_H__
