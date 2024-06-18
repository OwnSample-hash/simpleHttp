#ifndef __HTTP__LUA_
#define __HTTP__LUA_

#include "../log/log.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

extern lua_State *gL;

void lua_init(const char *root);
#define LUA_FUNCS_                                                             \
  REG(log_log)                                                                 \
  REG(scan_)                                                                   \
  REG(write_)

#define REG(fn) int lua_##fn(lua_State *gL);
LUA_FUNCS_
#undef REG

// p1 path str -3
// p2 meth str -2
// p3 retn function|str -1
// int lua_log_log(lua_State *L);

// p1 path str 1
// int lua_scan_(lua_State *L);

#endif // !__BYTES__HTTP_
