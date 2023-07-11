#include "warn.h"

int pipe_warn[2];
FILE *pipeW_writefp;
FILE *pipeW_readfp;

void warn(char *msg, ...) {
  va_list ap;
  va_start(ap, msg);
  vfprintf(pipeW_writefp, msg, ap);
  va_end(ap);
}

int init_warn() {
  if (pipe(pipe_warn) != 0) {
    perror("pipe, warn");
    return -1;
  }
  pipeW_writefp = fdopen(pipe_warn[1], "w");
  pipeW_readfp = fdopen(pipe_warn[0], "r");

  return 0;
}
