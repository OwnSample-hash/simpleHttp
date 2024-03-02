#include "quit_handler.h"
#include "log/log.h"
#include "socket.h"

open_socket open_sockets[MAX_OPEN_SOCKETS] = {0};
int open_sockets_len = 0;

void ctrl_c_h(int sigframe) {
  log_info("Quitting and freeing things... %d", getpid());
  lua_close(L);
  for (int i = 0; i < open_sockets_len; i++) {
    close(open_sockets[i].fd);
    log_debug("Closed socket %s:%d, fd: %d", open_sockets[i].conf->addr,
              open_sockets[i].conf->port, open_sockets[i].fd);
  }
  exit(0);
}
