// simpleHttp HTTP/1.1

#include "log/log.h"
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
  log_set_level(LOG_DEBUG);
#else
  log_set_quiet(1);
  log_set_level(LOG_FATAL);
#endif

  log_debug("hello %s %s", "mario", GIT_COMMIT);
  log_info("simpleHTTP-%s HTTP/1.1", GIT_COMMIT);
  log_info("http://127.0.0.1:%s/", (argc != 2) ? "8080" : argv[1]);
  createSocket(argc, argv);
  log_info("Listening on port %d", server_port);
  log_info("Starting to serve");
  serve();
  return 0;
}
