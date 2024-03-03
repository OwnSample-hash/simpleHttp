#ifndef __THREADJOB_HTTP__
#define __THREADJOB_HTTP__

#include "bytes.h"
#include "log/log.h"
#include "lua/setup.h"
#include "parser.h"
#include "socket.h"
#include <sys/wait.h>

int serve(int *fds, const driver *drv);
void threadJob(int client_sockfd, const char *server);

#endif
