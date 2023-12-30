#include "lua_.h"
#include <lua5.2/lauxlib.h>
#include <lua5.2/lua.h>

lua_State *L;
void lua_init() {
  L = luaL_newstate();
  luaL_openlibs(L);
  lua_pushstring(L, "GET");
  lua_setglobal(L, "GET");
  lua_pushcfunction(L, lua_reg_route);
  lua_setglobal(L, "reg_route");
  if (luaL_dostring(L, "require \"simpleHttpLua\"") != LUA_OK) {
    log_error("LUA ERROR");
    luaL_error(L, "Error %s\n", lua_tostring(L, -1));
  }
  log_info("Lua ran successfully.");
}

// p1 path str -3
// p2 meth str -2
// p3 retn function|str -1
int lua_reg_route(lua_State *L) {
  const char *path = lua_tostring(L, -3);
  const char *meth = lua_tostring(L, -2);
  const char *payl;
  if (lua_isfunction(L, -1)) {
    log_trace("Function");
    payl = "func";
  } else {
    log_trace("Str");
    payl = lua_tostring(L, -1);
  }
  log_debug("path:%s\tmeth:%s\tpayl:%s", path, meth, payl);
  return 0;
}
