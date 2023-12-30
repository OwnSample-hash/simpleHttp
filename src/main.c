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

  log_info("simpleHTTP-%s HTTP/1.1", GIT_COMMIT);
  log_debug("http://127.0.0.1:%s/",
            (argc != 2) ? "8080" : argv[1]); // talkin about cursed impl
  createSocket(argc, argv);
  lua_init();
  log_info("Listening on port %d", server_port);
  serve();
  return 0;
}
