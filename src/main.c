// simpleHttp HTTP/1.1

#include "linkList.h"
#include "logger/dbg.h"
#include "logger/logger.h"
#include "parser.h"
#include "quit_handler.h"
#include "socket.h"
#include "threadJob.h"
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#ifndef GIT_COMMIT
#define GIT_COMMIT "none-given"
#endif

int main(int argc, char **argv) {
  signal(SIGINT, ctrl_c_h);

#ifndef NO_LOG
  logConf_t logConf = {.lvl = DBG, .errQuit = false};
  if (init_logger(&logConf) != 0) {
    return 1;
  }
#else
  printf("Not inting logging shit\n");
  logConf_t logConf = {.lvl = LNONE, .fn = NULL, .fp = NULL, .errQuit = false};
  init_logger(&logConf);
#endif

  _DBG("hello %s %s\n", "mario", GIT_COMMIT);
  _INFO("simpleHTTP-%s HTTP/1.1\n", GIT_COMMIT);
  _INFO("http://127.0.0.1:%s/\n", (argc != 2) ? "8080" : argv[1]);
  createSocket(argc, argv);
  _INFO("Listening on port %d\n", server_port);
  pid_t serve_p = fork();
  serve_p == 0    ? serve()
  : serve_p == -1 ? perror("serve,fork")
                  : _INFO("serve pid %d", serve_p);
  handle();
  wait(NULL);
  return 0;
}
