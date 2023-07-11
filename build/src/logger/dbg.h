#ifndef __DBG_HTTP_
#define __DBG_HTTP_

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int pipe_dbg[2];
extern FILE *pipeD_writefp;
extern FILE *pipeD_readfp;
void dbg(char *msg, ...);
int init_dbg();

#endif // !__DBG_HTTP_
