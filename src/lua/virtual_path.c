#include "virtual_path.h"
#include "../mime_guess.h"
#include "../parser.h"
#include <lauxlib.h>
#include <lua.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

lookup_status virtual_path_resolv(const char *path, const int cfd) {
  log_info("Looking up path: %s", path);
  lua_getglobal(L, "_Funcs");
  lua_getfield(L, -1, path);
  if (!lua_isnil(L, -1)) {
    const char *payload;
    int payload_len;
    if (lua_isstring(L, -1)) {
      log_trace("L, -1 is a string, returning");
      payload = lua_tostring(L, -1);
      payload_len = strlen(payload);
    } else {
      log_trace("L, -1 is a func, calling");
      lua_call(L, 0, 1);
      payload = lua_tostring(L, -1);
      payload_len = strlen(payload);
      log_trace("From function call got string, with len of %d", payload_len);
    }
    char *cntTyp = ContentType(M_BUFFER, payload);
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
