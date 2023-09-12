#ifndef __THREADJOB_HTTP__
#define __THREADJOB_HTTP__

#include "logger/logger.h"
#include "parser.h"
#include "socket.h"
#include <sys/wait.h>

int serve();
void threadJob(int client_sockfd);
extern char buf[BUFSIZ];

#endif
