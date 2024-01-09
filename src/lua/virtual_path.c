#include "virtual_path.h"
#include "../mime_guess.h"
#include "../parser.h"
#include "lua_.h"
#include <lauxlib.h>
#include <lua.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MIME_LOOKUPS                                                           \
  EXPAND("json", "Content-Type: application/json\r\n\0")                       \
  EXPAND("html", "Content-Type: plain/html\r\n\0")

char *cntpLookup(const char *short_code) {
  char *ret;
#define EXPAND(sc, resp)                                                       \
  log_trace("Comapring short_code %s against sc: %s", short_code, sc);         \
  if (strncmp(short_code, #sc, strlen(#sc)) == 0) {                            \
    ret = calloc(strlen(#sc), sizeof(char));                                   \
    strcpy(ret, resp);                                                         \
  } else {                                                                     \
    log_trace("short code is not sc");                                         \
  }
  MIME_LOOKUPS
#undef EXPAND
  return ret;
}

lookup_status virtual_path_resolv(const char *path, const int cfd) {
  log_info("Looking up path: %s", path);
  lua_pushnumber(L, cfd);
  lua_setglobal(L, "__CFD");
  lua_getglobal(L, "_Funcs");
  lua_getfield(L, -1, path);
  if (!lua_isnil(L, -1)) {
    const char *payload;
    char *cntTyp;
    int payload_len;
    if (lua_isstring(L, -1)) {
      log_trace("L, -1 is a string, returning");
      payload = lua_tostring(L, -1);
      payload_len = strlen(payload);
    } else if (lua_istable(L, -1)) {
      log_trace("L, -1 is a table, parsing");
      lua_getfield(L, -1, "fn");
      luaL_checktype(L, -1, LUA_TFUNCTION);
      lua_call(L, 0, 1);
      payload = lua_tostring(L, -1);
      payload_len = strlen(payload);
      log_trace("From tbl[fn] call got string, with len of %d", payload_len);
      lua_getfield(L, -2, "cntp");
      log_trace("Checking if -1 is string ");
      luaL_checktype(L, -1, LUA_TSTRING);
      cntTyp = cntpLookup(lua_tostring(L, -1));
      if (cntTyp == NULL) {
        log_error("Failing back to auto MIME detection");
      }

    } else {
      log_trace("L, -1 is a func, calling");
      lua_call(L, 0, 1);
      payload = lua_tostring(L, -1);
      payload_len = strlen(payload);
      log_trace("From function call got string, with len of %d", payload_len);
    }
    if (strncmp(payload, "$!HANDELD", 9) == 0) {
      log_trace("Lua handeld respondig...");
      return HANDLED;
    }
    cntTyp = cntTyp == NULL ? ContentType(M_BUFFER, payload) : cntTyp;
    if (!cntTyp) {
      log_error("Failed to get content type");
    }
    log_trace("RETURNED");
    int cntLen = strlen(cntTyp);
    // first param const int is 21 working should be 37
    char *header_payload =
        calloc(35 + payload_len + strlen(HEADER) + cntLen, sizeof(char));

    snprintf(header_payload, 35 + payload_len + strlen(HEADER) + cntLen,
             "%s%s %d\r\n%s", HEADER, "Content-Length:", payload_len, cntTyp);
    write(cfd, header_payload, strlen(header_payload));
    write(cfd, "\r\n", 2);
    write(cfd, payload, payload_len);
    // write(cfd, "\r\n", 2);
    free(cntTyp);
    free(header_payload);
  } else {
    log_error("Path is not a virtual path(%s)", path);
    return NIL;
  }
  return OK_LOOKUP;
}
