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

extern ELogLevel curr_level;
int init_logger(const ELogLevel lvl, const char *fn);
void handle();

#endif // !__LOGGER_HTTP__
