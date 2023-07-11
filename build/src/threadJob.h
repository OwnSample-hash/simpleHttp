#ifndef __THREADJOB_HTTP__
#define __THREADJOB_HTTP__

#include "parser.h"
#include "socket.h"
#include <pthread.h>

void threadJob(int client_sockfd);
extern char buf[BUFSIZ];

typedef struct __threads {
  pthread_t *data;
  struct __threads *next;
} *threads_t;

#endif
