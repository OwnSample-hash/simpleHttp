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

#endif // !__WARN_HTTP_
