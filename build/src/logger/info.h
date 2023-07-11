#ifndef __INFO_HTTP_
#define __INFO_HTTP_

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int pipe_info[2];
extern FILE *pipeI_writefp;
extern FILE *pipeI_readfp;
void info(char *msg, ...);
int init_info();

#endif // !__INFO_HTTP_
