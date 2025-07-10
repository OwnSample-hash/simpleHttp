/**
 * @file
 * @brief Lua setup
 */

#ifndef __HTTP_SETUP__
#define __HTTP_SETUP__

#include "../log/log.h"
#include "../socket.h"
#include <lua.h>

/**
 * @typedef keep_alive_t
 * @brief Keep alive settings
 *
 * @var keep_alive::keep_alive
 * @brief Wheter to keep alive the connection or not
 *
 * @var keep_alive::timeout
 * @brief How long should we keep the connection before timing out, in ms
 *
 * @var keep_alive::max
 * @brief How many request can this conection server before closing it.
 */
typedef struct keep_alive {
  bool keep_alive;
  int timeout, max;
} keep_alive_t;

/**
 * @typedef driver_t
 * @brief Driver settings for the server
 *
 * @var _drv::socket
 * @brief The listening sockets that belongs to this driver
 *
 * @var _drv::socket_count
 * @brief The number of sockets
 *
 * @var _drv::log_level
 * @brief The level that we should log
 *
 * @var _drv::server_root
 * @brief The server root
 *
 * @var _drv::routes_root
 * @brief Where to load lua routes from
 *
 * @var _drv::keep_alive
 * @brief Keep alive settings
 */
typedef struct _drv {
  new_sock_t **socket;
  int socket_count;
  LOG_LEVEL log_level;
  char *server_root;
  char *routes_root;
  keep_alive_t keep_alive;
} driver_t;

/**
 * @brief Initialize the server
 *
 * @param conf_file Configuration file
 * @param drv Driver settings
 */
void init(const char *conf_file, driver_t *drv);

/**
 * @def LUA_FUNCS_INIT
 * @brief Lua functions
 */

#define LUA_FUNCS_INIT                                                         \
  REG(create_socket)                                                           \
  REG(set_server_root)                                                         \
  REG(set_routes_root)                                                         \
  REG(set_log_level)                                                           \
  REG(set_keep_alive)

/**
 * @def LVLS
 * @brief Log levels and address families
 */

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

/**
 * @def REG(fn)
 * @brief Register lua function
 * @param fn Function name
 *
 * @fn int lua_create_socket(lua_State *L)
 * @brief Create a socket
 * @param L Lua state
 * @return int
 *
 * @fn int lua_set_server_root(lua_State *L)
 * @brief Set the server root
 * @param L Lua state
 * @return int
 *
 * @fn int lua_set_routes_root(lua_State *L)
 * @brief Set the routes root
 * @param L Lua state
 * @return int
 *
 * @fn int lua_set_log_level(lua_State *L)
 * @brief Set the log level
 * @param L Lua state
 * @return int
 *
 * @fn int lua_set_keep_alive(lua_State *L)
 * @brief Set the keep alive settings
 * @param L Lua state
 * @return int
 */
#define REG(fn) int lua_##fn(lua_State *L);
LUA_FUNCS_INIT
#undef REG

#endif
