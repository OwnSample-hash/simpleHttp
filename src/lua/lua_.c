#include "lua_.h"
#include "../log/log.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <unistd.h>

lua_State *gL;
void lua_init(const char *routes, const char *server) {
  gL = luaL_newstate();
  luaL_openlibs(gL);
  lua_pushstring(gL, server);
  lua_setglobal(gL, "SERVER_ROOT");
  lua_pushstring(gL, routes);
  lua_setglobal(gL, "ROUTES_ROOT");

#define REG(fn)                                                                \
  lua_pushcfunction(gL, lua_##fn);                                             \
  lua_setglobal(gL, #fn);                                                      \
  log_trace("pushed lua_%s, and setted as global: %s", #fn, #fn);
  LUA_FUNCS_
#undef REG
  const char *lua_code = "local status, err = xpcall(function() require "
                         "'simpleHttpdLua' end, debug.traceback); "
                         "if not status then error(err) end;";
  if (luaL_dostring(gL, lua_code) != LUA_OK) {
    const char *error_msg = lua_tostring(gL, -1);

    log_error("LUA ERROR with stacktrace");
    luaL_error(gL, "Error: %s\n", error_msg);
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
// Vim: set expandtab tabstop=2 shiftwidth=2:
