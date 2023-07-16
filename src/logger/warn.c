#include "warn.h"

int pipe_warn[2];
FILE *pipeW_writefp;
FILE *pipeW_readfp;
bool inited_w = true;

void warn(char *msg, ...) {
  if (!inited_w) {
    return;
  }
  va_list ap;
  va_start(ap, msg);
  fprintf(pipeW_writefp, "[WARN] ");
  vfprintf(pipeW_writefp, msg, ap);
  va_end(ap);
}

int init_warn() {
  if (pipe(pipe_warn) != 0) {
    perror("pipe, warn");
    return -1;
  }
  inited_w = true;
  pipeW_writefp = fdopen(pipe_warn[1], "w");
  pipeW_readfp = fdopen(pipe_warn[0], "r");

  return 0;
}
