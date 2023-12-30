#include "lua_.h"
#include <lua5.2/lauxlib.h>
#include <lua5.2/lua.h>
#include <stdarg.h>

lua_State *L;
void lua_init() {
  L = luaL_newstate();
  luaL_openlibs(L);
  lua_pushglobaltable(L);
  lua_setglobal(L, "Funcs");
  lua_pushcfunction(L, lua_reg_route);
  lua_setglobal(L, "reg_route");
  lua_pushcfunction(L, lua_log_log);
  lua_setglobal(L, "log_log");
  if (luaL_dostring(L, "require \"simpleHttpLua\"") != LUA_OK) {
    log_error("LUA ERROR");
    luaL_error(L, "Error %s\n", lua_tostring(L, -1));
  }
  log_info("Lua ran successfully.");
}

int lua_log_log(lua_State *L) {
  int args = 0;
  if ((args = lua_gettop(L)) < 3) {
    log_error("Must pass 4 or more args, passed %d", args);
    return 0;
  }
  int level = lua_tonumber(L, 1);
  const char *file = lua_tostring(L, 2);
  int line = lua_tonumber(L, 3);
  const char *fmt = luaL_checkstring(L, 4);

  log_log(level, file, line, fmt);
  return 0;
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
