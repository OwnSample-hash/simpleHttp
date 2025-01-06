#include "virtual_path.h"
#include "../itoa.h"
#include "../mime_guess.h"
#include "../write_response.h"
#include "lua_.h"
#include <lauxlib.h>
#include <lua.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

lookup_status virtual_path_resolv(const char *path, const int cfd,
                                  const char *method,
                                  const keep_alive_t *keep_alive) {
  char *cntTyp;
  const char *payload;
  int payload_len;
  log_info("Looking up path: %s", path);
  lua_pushnumber(gL, cfd);
  lua_setglobal(gL, "__CFD");
  lua_getglobal(gL, "_Funcs");
  lua_getfield(gL, -1, path);
  if (!lua_isnil(gL, -1)) {
    if (lua_isstring(gL, -1)) {
      log_trace("L, -1 is a string, returning");
      payload = lua_tostring(gL, -1);
      payload_len = strlen(payload);
      cntTyp = NULL;
    } else if (lua_istable(gL, -1)) {
      log_trace("L, -1 is a table, parsing");
      lua_getfield(gL, -1, "meth");
      luaL_checktype(gL, -1, LUA_TTABLE);
      lua_pushnil(gL);
      while (lua_next(gL, -2) != 0) {
        const char *meth = lua_tostring(gL, -1);
        log_trace("Checking if %s == %s", meth, method);
        if (strcmp(meth, method) == 0) {
          log_trace("Found method");
          lua_getfield(gL, -4, "fn");
          luaL_checktype(gL, -1, LUA_TFUNCTION);
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
    }
    if (strncmp(payload, "$!HANDELD", 9) == 0) {
      log_trace("Lua handeld response...");
      return HANDLED;
    }
    cntTyp = cntTyp == NULL ? ContentType(M_BUFFER, payload) : cntTyp;
    if (!cntTyp) {
      log_error("Failed to get content type");
    }
    log_trace("RETURNED %s", cntTyp);
    response_t *resp = newRespones(cfd, lookupStatus(200), keep_alive);
    if (!resp) {
      log_error("Failed to create response");
      return NIL;
    }
    addHeader2Response(resp, "Content-Type", cntTyp);
    addHeader2Response(resp, "Content-Length", TO_BASE(payload_len, 10));
    setPayload(resp, payload, payload_len);
    log_trace("Wrote %ju byte to client %d", writeResponse(resp), cfd);
    freeResponse(resp);
    free(cntTyp);
  } else {
    log_error("Path is not a virtual path(%s)", path);
    return NIL;
  }
  return OK_LOOKUP;
}
