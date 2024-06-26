#ifndef __HTTP_SETUP__
#define __HTTP_SETUP__

#include "../log/log.h"
#include "../socket.h"
#include <lua.h>

typedef struct {
  bool keep_alive;
  int timeout, max;
} keep_alive_t;

typedef struct _drv {
  new_sock **socket;
  int socket_count;
  LOG_LEVEL log_level;
  char *server_root;
  char *routes_root;
  keep_alive_t keep_alive;
} driver;

void init(const char *conf_file, driver *drv);

#define LUA_FUNCS_INIT                                                         \
  REG(create_socket)                                                           \
  REG(set_server_root)                                                         \
  REG(set_routes_root)                                                         \
  REG(set_log_level)                                                           \
  REG(set_keep_alive)

#define LVLS                                                                   \
  ADD(LOG_TRACE)                                                               \
  ADD(LOG_DEBUG)                                                               \
  ADD(LOG_INFO)                                                                \
  ADD(LOG_WARN)                                                                \
  ADD(LOG_ERROR)                                                               \
  ADD(LOG_FATAL)

#define REG(fn) int lua_##fn(lua_State *L);
LUA_FUNCS_INIT
#undef REG

#endif
