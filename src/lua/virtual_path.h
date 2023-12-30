#ifndef __BYTES__V_PATH_
#define __BYTES__V_PATH_

#include "lua_.h"

typedef enum {
  OK_LOOKUP = 0,
  NIL,
} lookup_status;

lookup_status virtual_path_resolv(const char *path, const int cfd);
#endif
