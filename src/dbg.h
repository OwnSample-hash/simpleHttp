#ifndef __DBG_HTTP__
#define __DBG_HTTP__
#include "log/log.h"

#ifndef NO_DBG_ASSERT
#define dbg_assert(expr, msg)                                                  \
  if (expr) {                                                                  \
    log_debug("[!DBG_BRK!]" msg);                                              \
    asm("int 3");                                                              \
  }
#define dbg_assertm(expr, msg, ...)                                            \
  if (expr) {                                                                  \
    log_debug("[!DBG_BRK!]" msg, __VA_ARGS__);                                 \
    asm("int 3");                                                              \
  }
#else
#define dbg_assert(...)
#endif

#endif // !__DBG_HTTP__
