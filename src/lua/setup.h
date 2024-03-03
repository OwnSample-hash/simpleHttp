#ifndef __HTTP_DRIVER__
#define __HTTP_DRIVER__

#include "../log/log.h"
#include "../socket.h"
#include "lua_.h"

typedef struct _drv {
  new_sock **socket;
  int socket_count;
  LOG_LEVEL log_level;
  const char *server_root;
  const char *routes_root;
} driver;

void init(const char *conf_file, driver *drv);

#define LUA_FUNCS_INIT                                                         \
  REG(create_socket)                                                           \
  REG(set_server_root)                                                         \
  REG(set_routes_root)

#define REG(fn) int lua_##fn(lua_State *L);
LUA_FUNCS_INIT
#undef REG

#endif
