/**
 * @file
 * @brief Socket creation and management
 */

#ifndef __SOCKET_HTTP_
#define __SOCKET_HTTP_
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * @brief The protocol enum
 */
typedef enum { HTTP, HTTPS } protocol;

/**
 * @brief The ssl_conf struct
 */
typedef struct {
  const char *full_key;
} ssl_conf;

/**
 * @def MAX_ADDR_LEN
 * The socket address struct
 * @def MAX_OPEN_SOCKETS
 * The maximum number of open sockets
 */
#define MAX_ADDR_LEN 12

/**
 * @brief The socket address struct
 *
 * @var addr The address
 * @var port The port
 * @var domain The domain
 * @var listen The listen same as listen(2)
 * @var proto The protocol
 * @var ssl_conf The ssl configuration unused
 */
typedef struct {
  char *addr;
  const int port;
  const int domain;
  const int listen;
  const protocol proto;
  const ssl_conf *ssl_conf;
} new_sock_t;

#define MAX_OPEN_SOCKETS 4

/**
 * @brief The open socket struct
 *
 * @var fd The file descriptor
 * @var conf The socket configuration
 */
typedef struct {
  int fd;
  new_sock_t *conf;
} open_socket;

/**
 * @brief The open socket list
 */
extern open_socket open_sockets[MAX_OPEN_SOCKETS];

/**
 * @brief The open socket list length
 */
extern int open_sockets_len;

/**
 * @brief Creates a new socket
 *
 * @param sock The socket configuration
 * @return int The file descriptor
 */
int createSocket(new_sock_t *sock);

/**
 * @brief Gets the address and port from a socket address
 *
 * @param addr The socket address
 * @param ipBuffer The buffer to store the address
 * @param ipBufferLength The length of the buffer
 * @param port The port
 */
void getAddressAndPort(const struct sockaddr *addr, char *ipBuffer,
                       const size_t ipBufferLength, int *port);

/**
 * @brief Converts a protocol to a string
 *
 * @param proto The protocol
 * @return const char* The string representation of the protocol
 */
const char *prototoa(const protocol proto);

/**
 * @brief Converts a string to a protocol
 *
 * @param asciiz The string
 * @return int* The protocol
 */
int *atoproto(const char *asciiz);

/**
 * @brief Converts a domain to it's struct size
 *
 * @param flag The domain flag sockaddr(3type)
 * @return unsigned long The size of the domain
 */
unsigned long domtosize(int flag);
#endif /* __SOCKET_HTTP_ */
