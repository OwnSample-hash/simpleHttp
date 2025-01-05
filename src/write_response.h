#ifndef __WRITE_RESPONSE_H__
#define __WRITE_RESPONSE_H__

#include "bytes.h"
#include "lua/setup.h"
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
  const int p1;
  const char *p2;
} pair_t;

/**
 * @class map
 * @brief A map of a pair of integer and string and a string
 *
 */
typedef struct map {
  const pair_t key;
  const char *value;
} map_t;

/**
 * @brief Maps the status code to the status message
 */
static const map_t STATUS_MAP[] = {
    {{200, "200"}, "Ok"},
    {{201, "201"}, "Created"},
    {{204, "204"}, "No Content"},
    {{400, "400"}, "Bad Request"},
    {{404, "404"}, "Not Found"},
    {{500, "500"}, "Internal Server Error"},
    {{501, "501"}, "Not Implemented"},
    {{505, "505"}, "HTTP Version Not Supported"},
    {{0, NULL}, NULL},
};

/**
 * @class header
 * @brief A header struct
 *
 */
typedef struct header {
  const char *name;
  const char *value;
} header_t;

/**
 * @typedef response
 * @brief THe response opaque struct
 *
 */
typedef struct response response_t;

/**
 * @brief Sends the response
 *
 * @param res The response struct
 * @return int The number of bytes written
 */
size_t writeResponse(response_t *res);

[[deprecated("I have no idea what it does")]]
size_t genHeader2(const header_t *header, const map_t *status,
                  const keep_alive_t *keep_alive, char *dst);

/**
 * @brief Creates a new response
 *
 * @param cfd The client file descriptor
 * @param status The status code
 * @param keep_alive The keep alive struct
 * @return
 */
[[nodiscard("You have to free it later")]] response_t *
newRespones(int cfd, const map_t *status, const keep_alive_t *keep_alive);

/**
 * @brief Adds a header to the response
 *
 * @param res The response struct
 * @param name The header name
 * @param value The header value
 */
void addHeader2Response(response_t *res, const char *name, const char *value);

/*header_t *expandHeader(header_t *header, size_t size);*/

/**
 * @brief Trims the header in the response
 *
 * @param resp The response struct
 */
void trimResponse(response_t *resp);

/**
 * @brief Frees the response struct
 *
 * @param resp The response struct
 */
void freeResponse(response_t *resp);

/**
 * @brief Generate the base header
 *
 * @param dst The destination buffer
 * @param status The status code
 * @param keep_alive The keep alive struct
 * @return int The length of the header
 */
int genHeader(char **dst, const map_t *status, const keep_alive_t *keep_alive);

/**
 * @brief Set the payload
 *
 * @param res The response struct
 * @param payload The payload
 * @param len The length of the payload
 * @return size_t The length of the payload
 */
size_t setPayload(response_t *res, const char *payload, size_t len);

/**
 * @brief Lookup the status code
 *
 * @param code The status code
 * @return const map_t* The status code
 */
const map_t *lookupStatus(int code);
#endif // __WRITE_RESPONSE_H__
