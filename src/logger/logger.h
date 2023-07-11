#ifndef __LOGGER_HTTP__
#define __LOGGER_HTTP__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "dbg.h"
#include "err.h"
#include "info.h"
#include "warn.h"

enum log_level {
  INFO = 1,
  WARN,
  ERR,
  DBG,
};

// extern int pipe_warn[2];
// extern int pipe_err[2];
// extern int pipe_dbg[2];

int init_logger(enum log_level lvl);

// void warn(char *msg, ...);
// void err(char *msg, ...);
// void dbg(char *msg, ...);

#endif // !__LOGGER_HTTP__
