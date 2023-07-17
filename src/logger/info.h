#ifndef __INFO_HTTP_
#define __INFO_HTTP_

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int pipe_info[2];
extern FILE *pipeI_writefp;
extern FILE *pipeI_readfp;
void info(char *msg, ...);
int init_info();
void info_with_line_num(const char *pre, const char *__file__,
                        const int __line__, const char *msg, ...);

#define _INFO(msg, ...)                                                        \
  info_with_line_num("[INFO] [%s:%d] ", __FILE__, __LINE__, msg, __VA_ARGS__)

#endif // !__INFO_HTTP_
