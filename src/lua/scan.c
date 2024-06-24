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
static lua_State *l;
int _filter(const char *path, const stat_t *sb, int tflag, FTW_t *tfwbuf) {
  if (tflag == FTW_F) {
    log_trace("Path: %s with size %d", path, sb->st_size);
    lua_pushstring(l, path);
    luaL_checktype(l, -1, LUA_TSTRING);
    luaL_checktype(l, -2, LUA_TTABLE);
    lua_setfield(l, -2, TO_BASE(counter++, 10));
    return 0;
  } else {
    log_trace("Ignoring \"%s\" on level %d", path, tfwbuf->level);
    return 0;
  }
}

int lua_scan_(lua_State *L) {
  luaL_checkstring(L, 1);
  const char *path = lua_tostring(L, 1);
  counter = 0;
  lua_newtable(L);
  l = L;
  int n = nftw(path, _filter, 5, FTW_PHYS);
  if (n == -1) {
    log_error("nftw failed to scan dir");
    perror("nftw");
  } else
    log_trace("nftw found %d files, on path %s", counter, path);
  return 1;
}
