#include "err.h"

int pipe_err[2];
FILE *pipeE_writefp;
FILE *pipeE_readfp;
bool inited_e = false;
bool _quit = true;

void err_with_line_num(const char *pre, const char *__file__,
                       const int __line__, const char *msg, ...) {
  if (!inited_e) {
    return;
  }
  va_list ap;
  va_start(ap, msg);
  fprintf(pipeE_writefp, pre, __file__, __line__);
  vfprintf(pipeE_writefp, msg, ap);
  va_end(ap);
}

void err(char *msg, ...) {
  if (!inited_e) {
    return;
  }
  va_list ap;
  va_start(ap, msg);
  fprintf(pipeE_writefp, "[!ERR] ");
  vfprintf(pipeE_writefp, msg, ap);
  va_end(ap);
}

int init_err(bool quit) {
  _quit = quit;
  if (pipe(pipe_err) != 0) {
    perror("pipe, err");
    return -1;
  }
  pipeE_writefp = fdopen(pipe_err[1], "w");
  pipeE_readfp = fdopen(pipe_err[0], "r");
  inited_e = true;
  return 0;
}
