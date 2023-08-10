#pragma once
#ifndef __LOGGER_HTTP__
#define __LOGGER_HTTP__

#include <poll.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../bytes.h"
#include "dbg.h"
#include "err.h"
#include "info.h"
#include "warn.h"

typedef enum __log_level {
  LNONE,
  ERR,
  WARN,
  INFO,
  DBG,
} ELogLevel;

typedef struct __Log_conf {
  ELogLevel lvl;
  const char *fn;
  FILE *fp;
  bool errQuit;
} logConf_t;

extern ELogLevel curr_level;
extern struct pollfd *fds;
int init_logger(const logConf_t *conf);
void handle();

#endif // !__LOGGER_HTTP__
