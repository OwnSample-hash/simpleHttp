#ifndef __HTTP_CONFIG_H__
#define __HTTP_CONFIG_H__

#include "bytes.h"

/**
 *
 * @def BIN_NAME
 * @brief Containes the a hardcoded bin name and a git commit
 *
 * @def HEADER_BASE
 * @brief The base of response header
 *
 * @def HTTP_EMPTY_LINE
 * @brief And HTTP empty line
 *
 * @def HTTP_EMPTY_LINE_LEN
 * @brief Len of an empty line
 *
 * @def DEFAULT_LEN
 * @brief The default len for a buffer
 */

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
 * @def MAX_ADDR_LEN
 * The socket address struct
 * @def MAX_OPEN_SOCKETS
 * The maximum number of open sockets
 */
#define MAX_ADDR_LEN 12

#define MAX_OPEN_SOCKETS 4

/**
 * @brief The stack count
 */
#define STACK_COUNT 64

#endif // __HTTP_CONFIG_H__
