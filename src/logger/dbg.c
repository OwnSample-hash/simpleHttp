#include "dbg.h"

int pipe_dbg[2];
FILE *pipeD_writefp;
FILE *pipeD_readfp;
bool inited_d = false;

void dbg_with_line_num(const char *pre, const char *__file__,
                       const int __line__, const char *msg, ...) {
  if (!inited_d)
    return;
  va_list ap;
  va_start(ap, msg);
  fprintf(pipeD_writefp, pre, __file__, __line__);
  vfprintf(pipeD_writefp, msg, ap);
  fprintf(pipeD_writefp, "%c", 0x0);
  va_end(ap);
}

void dbg(char *msg, ...) {
  if (!inited_d)
    return;
  va_list ap;
  va_start(ap, msg);
  fprintf(pipeD_writefp, "[DBG] ");
  vfprintf(pipeD_writefp, msg, ap);
  va_end(ap);
}

int init_dbg() {
  if (pipe(pipe_dbg) != 0) {
    perror("pipe,dbg");
    return -1;
  }
  inited_d = true;
  pipeD_writefp = fdopen(pipe_dbg[1], "w");
  pipeD_readfp = fdopen(pipe_dbg[0], "r");
  return 0;
}
