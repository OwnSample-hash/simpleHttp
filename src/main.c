// simpleHttp HTTP/1.1

#include "dbg.h"
#include "log/log.h"
#include "lua/lua_.h"
#include "lua/setup.h"
#include "quit_handler.h"
#include "socket.h"
#include "threadJob.h"
#include <lua.h>
#include <signal.h>
#include <unistd.h>

#ifndef GIT_COMMIT
#define GIT_COMMIT "none-given"
#endif

const driver *drv;

int main(int argc, char **argv) {
  signal(SIGINT, ctrl_c_h);

#ifndef NO_LOG
  log_set_level(LOG_TRACE);
#else
#warning "No LOG"
  log_set_level(LOG_FATAL);
  log_set_quiet(1);
#endif

  drv = init("setup.lua");
  if (!drv) {
    log_error("Idk");
    return 1;
  }

  log_info("simpleHTTP-%s HTTP/1.1, with %s", GIT_COMMIT, LUA_RELEASE);
  int sfd = createSocket(drv->socket);
  if (sfd == -1) {
    log_fatal("Check above for more info");
    log_fatal("Socket creation failed quitting...");
    return -1;
  }
  log_info("Routes root:\"%s\"", drv->routes_root);
  log_info("Server root:\"%s\"", drv->server_root);
  lua_init(drv->routes_root);
  log_info("Url: %s://%s%s%s:%d/", prototoa(drv->socket->proto),
           drv->socket->domain == AF_INET6 ? "[" : "", drv->socket->addr,
           drv->socket->domain == AF_INET6 ? "]" : "", drv->socket->port);
  // dbg_assert(1 == 1, "BREAK");
  return 0;
}
