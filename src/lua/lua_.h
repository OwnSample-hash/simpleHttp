#ifndef __BYTES__LUA_
#define __BYTES__LUA_

#include "../log/log.h"
#include <lua5.2/lauxlib.h>
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>

extern lua_State *L;

void lua_init();

// p1 path str -3
// p2 meth str -2
// p3 retn function|str -1
int lua_reg_route(lua_State *L);

#endif // !__BYTES__HTTP_
