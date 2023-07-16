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

#define DBG_L_NOFMT(...) dbg("[%s:%d] %s", __FILE__, __LINE__, __VA_ARGS__)

#endif // !__DBG_HTTP_
