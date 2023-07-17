#ifndef __DBG_HTTP_
#define __DBG_HTTP_

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int pipe_dbg[2];
extern FILE *pipeD_writefp;
extern FILE *pipeD_readfp;
void dbg(char *msg, ...);
int init_dbg();
void dbg_with_line_num(const char *pre, const char *__file__,
                       const int __line__, const char *msg, ...);

#define _DBG(msg, ...)                                                         \
  dbg_with_line_num("[DBG]  [%s:%d] ", __FILE__, __LINE__, msg, __VA_ARGS__)

#endif // !__DBG_HTTP_
