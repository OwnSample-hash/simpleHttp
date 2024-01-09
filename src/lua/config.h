#ifndef __HTTP_CONFIG__
#define __HTTP_CONFIG__

#include "../socket.h"
#include "lua_.h"

typedef struct _conf {
  new_sock *socket;
} conf;

conf *readconf(const char *conf_file);

#endif
