#define _XOPEN_SOURCE 500
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
static int table_index = 0;
int _filter(const char *path, const stat_t *sb, int tflag, FTW_t *tfwbuf) {
  if (tflag == FTW_F) {
    log_trace("Path: %s with size %d", path, sb->st_size);
    lua_newtable(l);

    lua_pushstring(l, "size");
    lua_pushnumber(l, sb->st_size);
    lua_settable(l, -3);

    lua_pushstring(l, "mdate");
    lua_pushinteger(l, sb->st_mtime);
    lua_settable(l, -3);

    lua_pushstring(l, "path");
    lua_pushstring(l, path);
    lua_settable(l, -3);

    lua_rawseti(l, table_index, lua_rawlen(l, table_index) + 1);
    counter++;
    return 0;
  } else {
    log_trace("Ignoring \"%s\" on level %d", path, tfwbuf->level);
    return 0;
  }
}

int lua_scan_(lua_State *L) {
  luaL_checkstring(L, 1);
  luaL_checknumber(L, 2);
  const char *path = lua_tostring(L, 1);
  const int flag = lua_tonumber(L, 2);
  counter = 0;
  lua_newtable(L);
  table_index = lua_gettop(L);
  l = L;
  int n = nftw(path, _filter, 5, flag);
  if (n == -1) {
    log_error("nftw failed to scan dir");
    perror("nftw");
  } else
    log_trace("nftw found %d files, on path %s", counter, path);
  return 1;
}
