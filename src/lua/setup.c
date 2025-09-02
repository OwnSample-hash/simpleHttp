#include "setup.h"
#include "../plugin/plugin.h"
#include <lauxlib.h>
#include <linux/limits.h>
#include <lua.h>
#include <lualib.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int lua_create_socket(lua_State *L) {
  lua_getglobal(L, "__DRV");
  luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
  driver_t *drv = lua_touserdata(L, -1);
  if (!drv) {
    log_fatal("Failed to get drv");
    return 0;
  }
  if (drv->socket_count >= MAX_OPEN_SOCKETS) {
    log_fatal("Too many sockets, max is %d", MAX_OPEN_SOCKETS);
    lua_pushliteral(L, "Too many sockets, max is 4");
    lua_error(L);
  }
  luaL_checkstring(L, 1);  // IP
  luaL_checkinteger(L, 2); // PORT
  luaL_checkinteger(L, 3); // DOMAIN
  luaL_checkinteger(L, 4); // LISTEN
  luaL_checkinteger(L, 5); // PROTO
  const char *ip = lua_tostring(L, 1);
  int port = lua_tointeger(L, 2);
  int domain = lua_tonumber(L, 3);
  int listen = lua_tonumber(L, 4);
  int proto = lua_tonumber(L, 5);

  new_sock_t *ns = calloc(1, sizeof(new_sock_t));
  if (!ns) {
    perror("calloc,lua_create_socket");
    log_fatal("Failed to callocate memory for struct");
    lua_pushliteral(L, "Failed to callocate memroy for struct");
    lua_error(L);
  }

  ns->addr = calloc(MAX_ADDR_LEN, sizeof(char));
  strncpy((char *)ns->addr, ip, strlen(ip));
  memcpy((int *)&(ns->port), &port, sizeof(int));
  memcpy((int *)&(ns->domain), &domain, sizeof(int));
  memcpy((int *)&(ns->listen), &listen, sizeof(int));
  memcpy((int *)&(ns->proto), &proto, sizeof(int));

  drv->socket[drv->socket_count++] = ns;
  return 0;
}

int lua_set_server_root(lua_State *L) {
  luaL_checkstring(L, 1);
  lua_getglobal(L, "__DRV");
  luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
  driver_t *drv = lua_touserdata(L, -1);
  const char *tmp = lua_tostring(L, 1);
  drv->server_root = calloc(strlen(tmp) + 1, sizeof(char));
  snprintf(drv->server_root, strlen(tmp) + 1, "%s", tmp);
  return 0;
}

int lua_set_routes_root(lua_State *L) {
  luaL_checkstring(L, 1);
  lua_getglobal(L, "__DRV");
  luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
  driver_t *drv = lua_touserdata(L, -1);
  const char *tmp = lua_tostring(L, 1);
  drv->routes_root = calloc(strlen(tmp) + 1, sizeof(char));
  snprintf(drv->routes_root, strlen(tmp) + 1, "%s", tmp);
  return 0;
}

int lua_set_log_level(lua_State *L) {
  luaL_checkinteger(L, -1);
  log_set_level(lua_tonumber(L, -1));
  return 0;
}

int lua_set_keep_alive(lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  lua_getglobal(L, "__DRV");
  luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
  driver_t *drv = lua_touserdata(L, -1);
  lua_pushstring(L, "keep_alive");
  lua_gettable(L, 1);
  drv->keep_alive.keep_alive = lua_toboolean(L, -1);
  lua_pop(L, 1);
  lua_pushstring(L, "timeout");
  lua_gettable(L, 1);
  drv->keep_alive.timeout = lua_tonumber(L, -1);
  lua_pop(L, 1);
  lua_pushstring(L, "max");
  lua_gettable(L, 1);
  drv->keep_alive.max = lua_tonumber(L, -1);
  lua_pop(L, 1);
  log_trace("Keep-Alive: %d, To: %d, Max: %d", drv->keep_alive.keep_alive,
            drv->keep_alive.timeout, drv->keep_alive.max);
  return 0;
}

int lua_plugin_init(lua_State *L) {
  luaL_checktype(L, 1, LUA_TSTRING);
  lua_getglobal(L, "__DRV");
  luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
  driver_t *drv = lua_touserdata(L, -1);
  if (!drv) {
    log_fatal("Failed to get drv");
    return 0;
  }
  if (drv->plugins) {
    log_fatal("Plugin info already set, cannot set again");
    lua_pushliteral(L, "Plugin info already set, cannot set again");
    lua_error(L);
  }
  switch (plugin_system_init(lua_tostring(L, 1), &drv->plugins,
                             &drv->plugin_count)) {
  case HTTP_PLUGIN_INVALID:
    log_fatal("Invalid plugin directory");
    lua_pushliteral(L, "Invalid plugin directory");
    lua_error(L);
    break;
  case HTTP_PLUGIN_ERROR:
    log_fatal("Error initializing plugin system");
    lua_pushliteral(L, "Error initializing plugin system");
    lua_error(L);
    break;
  case HTTP_PLUGIN_OK:
    log_info("Plugin system initialized successfully with %zu plugins",
             drv->plugin_count);
    break;
  default:
    log_fatal("Unknown error initializing plugin system");
    lua_pushliteral(L, "Unknown error initializing plugin system");
    lua_error(L);
  }

  return 0;
}

void init(const char *conf_file, driver_t *drv) {
  lua_State *L_conf = luaL_newstate();
  luaL_openlibs(L_conf);
  lua_pushlightuserdata(L_conf, drv);
  lua_setglobal(L_conf, "__DRV");

#define ADD(value)                                                             \
  lua_pushnumber(L_conf, value);                                               \
  lua_setglobal(L_conf, #value);                                               \
  log_trace("[CONFIG] pushed %s, and setted as global: %s", #value, #value);
  LVLS
#undef ADD
  // clang-format off
#define REG(fn)                                                                \
  lua_pushcfunction(L_conf, lua_##fn);                                         \
  lua_setglobal(L_conf, #fn);                                                  \
  log_trace("[CONFIG] pushed lua_%s, and setted as global: %s", #fn, #fn);
  LUA_FUNCS_INIT
#undef REG

  const char *lua_code_fmt = "local status, err = xpcall(function() local mod="
                         "string.gsub('%s', '.lua', ''); require(mod) end, "
                         "debug.traceback);"
                         "if not status then error(err) end;";
  char* lua_code = calloc(strlen(lua_code_fmt) + strlen(conf_file) + 1,
                          sizeof(char));
  if (!lua_code) {
    log_fatal("Failed to allocate memory for lua_code");
    return;
  }
  snprintf(lua_code, strlen(lua_code_fmt) + strlen(conf_file) + 1, lua_code_fmt,
           conf_file);
  if (luaL_dostring(L_conf, lua_code) != LUA_OK) {
    const char *error_msg = lua_tostring(L_conf, -1);

    log_error("LUA ERROR with stacktrace");
    luaL_error(L_conf, "Error: %s\n", error_msg);
  }
  free(lua_code);
  lua_close(L_conf);
}
// Vim: set expandtab tabstop=2 shiftwidth=2:
