#include "lua_.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <unistd.h>

lua_State *L;
void lua_init() {
  L = luaL_newstate();
  luaL_openlibs(L);
  // lua_pushglobaltable(L);
  // lua_setglobal(L, "_Funcs");

#define REG(fn)                                                                \
  lua_pushcfunction(L, lua_##fn);                                              \
  lua_setglobal(L, #fn);                                                       \
  log_trace("pushed lua_%s, and setted as global: %s", #fn, #fn);
  LUA_FUNCS_
#undef REG

  if (luaL_dostring(L, "require \"simpleHttpdLua\"") != LUA_OK) {
    log_error("LUA ERROR");
    luaL_error(L, "Error %s\n", lua_tostring(L, -1));
  }
  log_info("Lua ran successfully.");
}

int lua_write_(lua_State *L) {
  luaL_checknumber(L, 1);
  luaL_checkstring(L, 2);
  luaL_checknumber(L, 3);
  int cfd = lua_tonumber(L, 1);
  const char *data = lua_tostring(L, 2);
  int len = lua_tonumber(L, 3);
  log_trace("Writing data to client: %d with len %d", cfd, len);
  int bytes = write(cfd, data, len);
  if (bytes < 0) {
    perror("lua_write_");
    log_fatal("Failed to write data to client: %d", cfd);
  }
  lua_pushnumber(L, bytes);
  return 1;
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
