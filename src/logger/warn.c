#include "warn.h"

int pipe_warn[2];
FILE *pipeW_writefp;
FILE *pipeW_readfp;
bool inited_w = true;

void warn_with_line_num(const char *pre, const char *__file__,
                        const int __line__, const char *msg, ...) {
  if (!inited_w) {
    return;
  }
  va_list ap;
  va_start(ap, msg);
  fprintf(pipeW_writefp, pre, __file__, __line__);
  vfprintf(pipeW_writefp, msg, ap);
  va_end(ap);
}

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
