/**
 * @file lua_.h
 * @brief Lua functions
 */

#ifndef __HTTP__LUA_
#define __HTTP__LUA_

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

/**
 * @brief Global lua state
 */
extern lua_State *gL;

/**
 * @brief Initialize lua state
 *
 * @param routes Path for the virtual routes
 * @param server Path for the server root
 */
void lua_init(const char *routes, const char *server);

/**
 * @def LUA_FUNCS_
 * @brief Lua functions
 *
 * @def REG(fn)
 * @brief Register lua function
 */

#define LUA_FUNCS_                                                             \
  REG(log_log)                                                                 \
  REG(scan_)                                                                   \
  REG(write_)

/**
 * @fn int lua_log_log(lua_State *L)
 * @brief Log to the log file
 * @param L Lua state
 * @return int
 *
 * @fn int lua_scan_(lua_State *L)
 * @brief Scan a directory
 * @param L Lua state
 * @return int
 *
 * @fn int lua_write_(lua_State *L)
 * @brief Write to a client
 * @param L Lua state
 * @return int
 */

#define REG(fn) int lua_##fn(lua_State *gL);
LUA_FUNCS_
#undef REG

#endif // !__BYTES__HTTP_
