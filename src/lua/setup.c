#include "setup.h"
#include <lauxlib.h>
#include <linux/limits.h>
#include <lua.h>
#include <lualib.h>
#include <stdlib.h>
#include <string.h>

int lua_create_socket(lua_State *L) {
  // create_socket(IP, PORT, DOMAIN, LISTEN, PROTO)
  luaL_checkstring(L, 1);  // IP
  luaL_checkinteger(L, 2); // PORT
  luaL_checkstring(L, 3);  // DOMAIN
  luaL_checkinteger(L, 4); // LISTEN
  luaL_checkstring(L, 5);  // PROTO
  const char *ip = lua_tostring(L, 1);
  int port = lua_tointeger(L, 2);
  const char *domain = lua_tostring(L, 3);
  int listen = lua_tonumber(L, 4);
  const char *proto = lua_tostring(L, 5);

  new_sock *ns = calloc(1, sizeof(new_sock));
  if (!ns) {
    perror("calloc,lua_create_socket");
    log_fatal("Failed to callocate memory for struct");
    lua_pushliteral(L, "Failed to callocate memroy for struct");
    lua_error(L);
  }

  lua_getglobal(L, "__DRV");
  luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
  driver *drv = lua_touserdata(L, -1);
  if (!drv) {
    log_fatal("Failed to get drv");
    return 0;
  }
  ns->addr = calloc(MAX_ADDR_LEN, sizeof(char));
  strncpy((char *)ns->addr, ip, strlen(ip));
  memcpy((int *)&(ns->port), &port, sizeof(int));

#define DOMAINS_                                                               \
  X("AF_INET", AF_INET)                                                        \
  X("AF_INET6", AF_INET6)

#define X(str, domain_)                                                        \
  if (strncmp(str, domain, strlen(str)) == 0) {                                \
    int tmp = domain_;                                                         \
    memcpy((int *)&(ns->domain), &tmp, sizeof(int));                           \
  }
  DOMAINS_
#undef X
  memcpy((int *)&(ns->listen), &listen, sizeof(int));
  int *proto_ = atoproto(proto);
  memcpy((int *)&(ns->proto), proto_, sizeof(int));
  free(proto_);

  drv->socket[drv->socket_count++] = ns;
  return 0;
}

int lua_set_server_root(lua_State *L) {
  luaL_checkstring(L, 1);
  lua_getglobal(L, "__DRV");
  luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
  driver *drv = lua_touserdata(L, -1);
  const char *tmp = lua_tostring(L, 1);
  drv->server_root = calloc(strlen(tmp), sizeof(char));
  strncpy((char *)drv->server_root, tmp, strlen(tmp));
  return 0;
}

int lua_set_routes_root(lua_State *L) {
  luaL_checkstring(L, 1);
  lua_getglobal(L, "__DRV");
  luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
  driver *drv = lua_touserdata(L, -1);
  const char *tmp = lua_tostring(L, 1);
  drv->routes_root = calloc(strlen(tmp), sizeof(char));
  strncpy((char *)drv->routes_root, tmp, strlen(tmp));
  return 0;
}

void init(const char *conf_file, driver *drv) {
  lua_State *L_conf = luaL_newstate();
  luaL_openlibs(L_conf);
  // driver *drv = calloc(1, sizeof(driver));
  lua_pushlightuserdata(L_conf, drv);
  lua_setglobal(L_conf, "__DRV");

#define REG(fn)                                                                \
  lua_pushcfunction(L_conf, lua_##fn);                                         \
  lua_setglobal(L_conf, #fn);                                                  \
  log_trace("pushed lua_%s, and setted as global: %s", #fn, #fn);
  LUA_FUNCS_INIT
#undef REG

  if (luaL_dofile(L_conf, conf_file) != LUA_OK) {
    log_fatal("Failed to load config file");
    luaL_error(L_conf, "Error %s\n", lua_tostring(L_conf, -1));
  }

  lua_close(L_conf);
  // return drv;
}
