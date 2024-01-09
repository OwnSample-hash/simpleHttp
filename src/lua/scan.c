#define _XOPEN_SOURCE 500
#include "../itoa.h"
#include "lua_.h"
#include <dirent.h>
#include <ftw.h>
#include <lauxlib.h>
#include <lua.h>
#include <stdio.h>

typedef struct stat stat_t;
typedef struct FTW FTW_t;
static int counter = 0;
int _filter(const char *path, const stat_t *sb, int tflag, FTW_t *tfwbuf) {
  if (tflag == FTW_F) {
    log_trace("Path: %s", path);
    lua_pushstring(L, path);
    luaL_checktype(L, -1, LUA_TSTRING);
    luaL_checktype(L, -2, LUA_TTABLE);
    lua_setfield(L, -2, TO_BASE(counter++, 10));
    return 0;
  } else {
    log_trace("Ignoring \"%s\"", path);
    return 0;
  }
}

int lua_scan_(lua_State *L) {
  luaL_checkstring(L, 1);
  const char *path = lua_tostring(L, 1);
  counter = 0;
  lua_newtable(L);
  int n = nftw(path, _filter, 5, FTW_PHYS);
  if (n == -1) {
    log_error("dirent failed to scan dir");
    perror("dirent");
  } else
    log_trace("nftw found %d files, on path %s", counter, path);
  return 1;
}
