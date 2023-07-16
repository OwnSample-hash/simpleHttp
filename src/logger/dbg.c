#include "dbg.h"

int pipe_dbg[2];
FILE *pipeD_writefp;
FILE *pipeD_readfp;
bool inited_d = false;

void dbg(char *msg, ...) {
  if (!inited_d) {
    return;
  }
  va_list ap;
  va_start(ap, msg);
  fprintf(pipeD_writefp, "[DBG] ");
  vfprintf(pipeD_writefp, msg, ap);
  va_end(ap);
}

int init_dbg() {
  if (pipe(pipe_dbg) != 0) {
    perror("pipe, dbg");
    return -1;
  }
  inited_d = true;
  pipeD_writefp = fdopen(pipe_dbg[1], "w");
  pipeD_readfp = fdopen(pipe_dbg[0], "r");
  return 0;
}
