#include "info.h"

int pipe_info[2];
FILE *pipeI_writefp;
FILE *pipeI_readfp;
bool inited_i = false;

void info(char *msg, ...) {
  if (!inited_i) {
    return;
  }
  va_list ap;
  va_start(ap, msg);
  fprintf(pipeI_writefp, "[INFO] ");
  vfprintf(pipeI_writefp, msg, ap);
  va_end(ap);
}

int init_info() {
  if (pipe(pipe_info) != 0) {
    perror("pipe, info");
    return -1;
  }
  inited_i = true;
  pipeI_writefp = fdopen(pipe_info[1], "w");
  pipeI_readfp = fdopen(pipe_info[0], "r");
  return 0;
}
