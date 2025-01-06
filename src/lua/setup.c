#include "setup.h"
#include <lauxlib.h>
#include <linux/limits.h>
#include <lua.h>
#include <lualib.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int lua_create_socket(lua_State *L) {
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

  lua_getglobal(L, "__DRV");
  luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
  driver_t *drv = lua_touserdata(L, -1);
  if (!drv) {
    log_fatal("Failed to get drv");
    return 0;
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
  drv->server_root = calloc(strlen(tmp), sizeof(char));
  strncpy((char *)drv->server_root, tmp, strlen(tmp));
  return 0;
}

int lua_set_routes_root(lua_State *L) {
  luaL_checkstring(L, 1);
  lua_getglobal(L, "__DRV");
  luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
  driver_t *drv = lua_touserdata(L, -1);
  const char *tmp = lua_tostring(L, 1);
  drv->routes_root = calloc(strlen(tmp), sizeof(char));
  strncpy((char *)drv->routes_root, tmp, strlen(tmp));
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

  if (luaL_dofile(L_conf, conf_file) != LUA_OK) {
    log_fatal("Failed to load config file");
    luaL_error(L_conf, "Error %s\n", lua_tostring(L_conf, -1));
  }
  lua_close(L_conf);
}
