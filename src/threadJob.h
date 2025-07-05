#ifndef __THREADJOB_HTTP__
#define __THREADJOB_HTTP__

#include "lua/setup.h"
#include <stdbool.h>
#include <sys/wait.h>

/**
 * @enum quit_status_e
 * @brief The status of the thread
 */
typedef enum {
  THREAD_FAIL = -1,
  THREAD_OK,
  THREAD_SOCKET_HUNGUP,
  THREAD_TIMEOUT,
} quit_status_e;

/**
 * @typedef quit_status_t
 * Unsed up for removal
 */
typedef struct {
  int cfd;
  keep_alive_t *keep_alive;
} quit_status_t;

/**
 * @brief The main function for the thread
 *
 * @param client_sockfd The client socket file descriptor
 * @param server The main server root
 * @param keep_alive Keep alive parameters
 */
void threadJob(int client_sockfd, const char *server,
               const keep_alive_t *keep_alive);

#endif
