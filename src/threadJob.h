#ifndef __THREADJOB_HTTP__
#define __THREADJOB_HTTP__

#include "lua/setup.h"
#include <stdbool.h>
#include <sys/wait.h>

int serve(const driver *drv);
void threadJob(int client_sockfd, const char *server,
               const keep_alive_t *keep_alive);

#endif
