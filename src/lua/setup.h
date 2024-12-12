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
  char **methods;
  keep_alive_t keep_alive;
} driver_t;

void init(const char *conf_file, driver_t *drv);

#define LUA_FUNCS_INIT                                                         \
  REG(create_socket)                                                           \
  REG(set_server_root)                                                         \
  REG(set_routes_root)                                                         \
  REG(set_log_level)                                                           \
  REG(set_keep_alive)                                                          \
  REG(add_http_method)

#define LVLS                                                                   \
  ADD(LOG_TRACE)                                                               \
  ADD(LOG_DEBUG)                                                               \
  ADD(LOG_INFO)                                                                \
  ADD(LOG_WARN)                                                                \
  ADD(LOG_ERROR)                                                               \
  ADD(LOG_FATAL)                                                               \
  ADD(AF_INET)                                                                 \
  ADD(AF_INET6)                                                                \
  ADD(HTTP)                                                                    \
  ADD(HTTPS)

#define REG(fn) int lua_##fn(lua_State *L);
LUA_FUNCS_INIT
#undef REG

#endif
