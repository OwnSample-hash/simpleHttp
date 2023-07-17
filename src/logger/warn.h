#ifndef __WARN_HTTP_
#define __WARN_HTTP_

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int pipe_warn[2];
extern FILE *pipeW_writefp;
extern FILE *pipeW_readfp;
void warn(char *msg, ...);
int init_warn();
void warn_with_line_num(const char *pre, const char *__file__,
                        const int __line__, const char *msg, ...);

#define _WARN(msg, ...)                                                        \
  warn_with_line_num("[WARN] [%s:%d] ", __FILE__, __LINE__, msg, __VA_ARGS__)

#endif // !__WARN_HTTP_
