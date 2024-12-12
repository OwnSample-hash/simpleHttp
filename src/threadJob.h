#ifndef __THREADJOB_HTTP__
#define __THREADJOB_HTTP__

#include "lua/setup.h"
#include <mqueue.h>
#include <stdbool.h>
#include <sys/wait.h>

typedef enum {
  THREAD_FAIL = -1,
  THREAD_OK,
  THREAD_SOCKET_HUNGUP,
  THREAD_TIMEOUT,
} quit_status_e;

typedef struct {
  int cfd;
  mqd_t mq;
  keep_alive_t *keep_alive;
} quit_status_t;

int serve(const driver_t *drv);
void threadJob(int client_sockfd, const char *server,
               const keep_alive_t *keep_alive);

#endif
