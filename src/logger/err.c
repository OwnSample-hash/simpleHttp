#include "err.h"

int pipe_err[2];
FILE *pipeE_writefp;
FILE *pipeE_readfp;

void err(char *msg, ...) {
  va_list ap;
  va_start(ap, msg);
  fprintf(pipeE_writefp, "[!ERR] ");
  vfprintf(pipeE_writefp, msg, ap);
  va_end(ap);
}

int init_err() {
  if (pipe(pipe_err) != 0) {
    perror("pipe, err");
    return -1;
  }
  pipeE_writefp = fdopen(pipe_err[1], "w");
  pipeE_readfp = fdopen(pipe_err[0], "r");
  return 0;
}
