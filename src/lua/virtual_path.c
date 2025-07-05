#include "virtual_path.h"
#include "../itoa.h"
#include "../mime_guess.h"
#include "../write_response.h"
#include "lua_.h"
#include <fnmatch.h>
#include <lauxlib.h>
#include <lua.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

vlookup_status virtual_path_resolv(const char *path, const int cfd,
                                   const char *method,
                                   const keep_alive_t *keep_alive) {
  char *cntTyp;
  const char *payload;
  int payload_len;
  log_info("Looking up path: %s", path);
  lua_pushnumber(gL, cfd);
  lua_setglobal(gL, "__CFD");
  lua_pushstring(gL, method);
  lua_setglobal(gL, "__METHOD");
  lua_pushstring(gL, path);
  lua_setglobal(gL, "__PATH");
  lua_getglobal(gL, "_Funcs");
  if (lua_isnil(gL, -1)) {
    log_error("No _Funcs table found in Lua state");
    return NIL;
  }
  size_t idx = 0;
  if (lua_istable(gL, -1)) {
    size_t len = lua_rawlen(gL, -1);
    log_trace("Found _Funcs table with %zu entries", len);
    for (size_t i = 1; i <= len && idx == 0; i++) {
      lua_rawgeti(gL, -1, i);
      if (lua_istable(gL, -1)) {
        lua_getfield(gL, -1, "path");
        luaL_checktype(gL, -1, LUA_TSTRING);
        const char *tbl_path = lua_tostring(gL, -1);
        log_trace("Checking if %s matches %s", tbl_path, path);
        if (fnmatch(tbl_path, path, FNM_NOESCAPE) == 0) {
          log_trace("Match found for %s", tbl_path);
          idx = i;
          break;
        } else {
          log_trace("No match for %s", tbl_path);
        }
        lua_pop(gL, 1); // pop field
      }
      lua_pop(gL, 1); // pop table
    }
  }
  if (idx == 0) {
    log_error("No matching path found in _Funcs table for %s", path);
    return NIL;
  }
  log_trace("Found matching path at index %zu", idx);
  lua_getglobal(gL, "_Funcs");
  lua_rawgeti(gL, -1, idx);
  lua_getfield(gL, -1, "meth");
  luaL_checktype(gL, -1, LUA_TTABLE);
  lua_pushnil(gL);
  while (lua_next(gL, -2) != 0) {
    const char *meth = lua_tostring(gL, -1);
    log_trace("Checking if %s == %s", meth, method);
    if (strcmp(meth, method) == 0) {
      log_trace("Found method");
      lua_getglobal(gL, "_Funcs");
      lua_rawgeti(gL, -1, idx);
      lua_getfield(gL, -1, "handler");
      // luaL_checktype(gL, -1, LUA_TFUNCTION);
      if (!lua_isfunction(gL, -1)) {
        log_error("Handler is not a function");
        lua_pop(gL, 2); // pop handler and meth
        return NIL;
      }
       lua_call(gL, 0, 1);
      if (lua_isstring(gL, -1)) {
        log_trace("From tbl[meth][fn] call got string");
      } else {
        log_trace("From tbl[meth][fn] call got %s",
                  lua_typename(gL, lua_type(gL, -1)));
      }
      payload = lua_tostring(gL, -1);
      payload_len = strlen(payload);
      log_trace("From tbl[meth][fn] call got string, with len of %d",
                payload_len);
      cntTyp = NULL;
      break;
    } else {
      log_trace("No match");
      continue;
    }
    lua_pop(gL, 1);
  }
  log_trace("Done parsing table");
  lua_pop(gL, 1);
  // }
  if (strncmp(payload, "$!HANDELD", 9) == 0) {
    log_trace("Lua handeld response...");
    return HANDLED;
  }
  if (strncmp(payload, "$!STATIC", 8) == 0) {
    log_trace("Static response");
    return STATIC;
  }
  cntTyp = cntTyp == NULL ? ContentType(M_BUFFER, payload) : cntTyp;
  if (!cntTyp) {
    log_error("Failed to get content type");
  }
  log_trace("RETURNED %s", cntTyp);
  response_t *resp = new_response(cfd, lookup_status(200), keep_alive);
  if (!resp) {
    log_error("Failed to create response");
    return NIL;
  }
  add_header(resp, "Content-Type", cntTyp);
  add_header(resp, "Content-Length", TO_BASE(payload_len, 10));
  set_payload(resp, payload, payload_len);
  log_trace("Wrote %ju byte to client %d", write_response(resp), cfd);
  free_response(resp);
  free(cntTyp);

  return OK_LOOKUP;
}
// Vim: set expandtab tabstop=2 shiftwidth=2:
