#ifndef __ERR_HTTP_
#define __ERR_HTTP_

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int pipe_err[2];
extern FILE *pipeE_writefp;
extern FILE *pipeE_readfp;
void err(char *msg, ...);
int init_err();

#endif // !__ERR_HTTP_
