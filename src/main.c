// simpleHttp HTTP/1.1

#include "log/log.h"
#include "lua/lua_.h"
#include "quit_handler.h"
#include "socket.h"
#include "threadJob.h"
#include <signal.h>
#include <unistd.h>

#ifndef GIT_COMMIT
#define GIT_COMMIT "none-given"
#endif

int main(int argc, char **argv) {
  signal(SIGINT, ctrl_c_h);

#ifndef NO_LOG
  log_set_level(LOG_TRACE);
#else
#warning "No LOG"
  log_set_level(LOG_FATAL);
  log_set_quiet(1);
#endif

  new_sock ns = {.addr = "127.0.0.1",
                 .port = 8080,
                 .domain = AF_INET,
                 .listen = 5,
                 .proto = HTTP};

  log_info("simpleHTTP-%s HTTP/1.1", GIT_COMMIT);
  int sfd = createSocket(&ns);
  if (sfd == -1) {
    log_fatal("Check above for more info");
    log_fatal("Socket creation failed quitting...");
    return -1;
  }
  lua_init();
  log_info("Url: %s://%s:%d/", prototoa(ns.proto), ns.addr, ns.port);
  serve(sfd);
  return 0;
}
