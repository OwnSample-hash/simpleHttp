// simpleHttp HTTP/1.1

#include "log/log.h"
#include "lua/lua_.h"
#include "lua/setup.h"
#include "quit_handler.h"
#include "socket.h"
#include "threadJob.h"
#include <lua.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef GIT_COMMIT
#define GIT_COMMIT "none-given"
#endif

driver *drv;

int main(int argc, char **argv) {
  log_info("%s-%s HTTP/1.1, with %s, with %d args", argv[0], GIT_COMMIT,
           LUA_RELEASE, argc);
  signal(SIGINT, ctrl_c_h);
  drv = calloc(1, sizeof(driver));
  drv->socket = calloc(MAX_OPEN_SOCKETS, sizeof(new_sock));

#ifndef NO_LOG
  log_set_level(LOG_TRACE);
#else
#warning "No LOG"
  log_set_quiet(1);
#endif

  init("setup.lua", drv);
  if (!drv) {
    log_error("Idk");
    return 1;
  }

  int fds[MAX_OPEN_SOCKETS];
  for (int i = 0; i < drv->socket_count; i++)
    fds[i] = createSocket(drv->socket[i]);
  log_info("Routes root:\"%s\"", drv->routes_root);
  log_info("Server root:\"%s\"", drv->server_root);
  lua_init(drv->routes_root);
  for (int i = 0; i < drv->socket_count; i++)
    log_info(
        "Url: %s://%s%s%s:%d/", prototoa(drv->socket[i]->proto),
        drv->socket[i]->domain == AF_INET6 ? "[" : "", drv->socket[i]->addr,
        drv->socket[i]->domain == AF_INET6 ? "]" : "", drv->socket[i]->port);
  serve(drv);
  return 0;
}
