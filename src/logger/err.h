#ifndef __ERR_HTTP_
#define __ERR_HTTP_

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int pipe_err[2];
extern FILE *pipeE_writefp;
extern FILE *pipeE_readfp;
void err(char *msg, ...);
int init_err(bool quit);
void err_with_line_num(const char *pre, const char *__file__,
                       const int __line__, const char *msg, ...);

#define _ERR(msg, ...)                                                         \
  err_with_line_num("[ERR]  [%s:%d] ", __FILE__, __LINE__, msg, __VA_ARGS__)

#endif // !__ERR_HTTP_
