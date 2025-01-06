#ifndef __HTTP__V_PATH__
#define __HTTP__V_PATH__

#include "lua_.h"

typedef enum {
  OK_LOOKUP = 0,
  NIL,
  HANDLED,
} lookup_status;

lookup_status virtual_path_resolv(const char *path, const int cfd);
lookup_status virtual_path_resolv(const char *path, const int cfd,
                                  const char *method,
                                  const keep_alive_t *keep_alive);
#endif
