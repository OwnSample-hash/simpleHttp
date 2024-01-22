#ifndef __DBG_HTTP__
#define __DBG_HTTP__
#include "log/log.h"

#define dbg_assert(expr, msg, ...)                                             \
  if (expr) {                                                                  \
    log_debug("[!DBG_BRK!]" msg, __VA_ARGS__);                                 \
    asm("int 3");                                                              \
  }

#endif // !__DBG_HTTP__
