#include "config.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>

conf *readconf(const char *conf_file) {
  conf *ret = calloc(1, sizeof(conf));
  ret->socket = calloc(1, sizeof(new_sock));
  ret->socket->ssl_conf = calloc(1, sizeof(ssl_conf));
  if (!ret) {
    perror("calloc");
    log_fatal("Failed to callocatte memory to conf struct");
    return NULL;
  }
  lua_State *L_conf = luaL_newstate();
  luaL_openlibs(L_conf);

  char *conf_lua = calloc(strlen(conf_file) + 7, sizeof(char));
  snprintf(conf_lua, strlen(conf_file), "dofile %s", conf_file);

  if (luaL_dostring(L_conf, conf_lua) != LUA_OK) {
    log_fatal("Failed to load confif file");
    luaL_error(L_conf, "Error %s\n", lua_tostring(L, -1));
  }

  lua_close(L_conf);
  return ret;
}

void freeconf(conf *_conf) {
  free((void *)_conf->socket->ssl_conf);
  free(_conf->socket);

  free(_conf);
}
