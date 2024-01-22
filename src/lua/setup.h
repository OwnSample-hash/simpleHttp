#ifndef __HTTP_DRIVER__
#define __HTTP_DRIVER__

#include "../log/log.h"
#include "../socket.h"
#include "lua_.h"

typedef struct _drv {
  new_sock *socket;
  LOG_LEVEL log_level;
  const char *server_root;
  const char *routes_root;
} driver;

const driver *init(const char *conf_file);

#define LUA_FUNCS_INIT                                                         \
  REG(create_socket)                                                           \
  REG(set_server_root)                                                         \
  REG(set_routes_root)

#define REG(fn) int lua_##fn(lua_State *L);
LUA_FUNCS_INIT
#undef REG

#endif
