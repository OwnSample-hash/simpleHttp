#ifndef __THREADJOB_HTTP__
#define __THREADJOB_HTTP__

#include "bytes.h"
#include "log/log.h"
#include "parser.h"
#include "socket.h"
#include <sys/wait.h>

int serve(int sfd);
void threadJob(int client_sockfd);
extern char buf[BUFSIZ];

#endif
