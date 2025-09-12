#include "quit_handler.h"
#include "log/log.h"
#include "lua/lua_.h"
#include "plugin/plugin.h"
#include "socket.h"
#include "thread_pool.h"
#include <lua.h>
#include <pthread.h>
#include <stdlib.h>

open_socket_t open_sockets[MAX_OPEN_SOCKETS] = {0};
int open_sockets_len = 0;

void ctrl_c_h(int sigframe) {
  log_info("Quitting and freeing things, %d, with sigframe %d", getpid(),
           sigframe);
  if (sigframe == -1)
    log_trace("Called from a child!");

  lua_close(gL);
  for (int i = 0; i < open_sockets_len; i++) {
    close(open_sockets[i].fd);
    log_info("Closed socket %s:%d, fd: %d", open_sockets[i].conf->addr,
             open_sockets[i].conf->port, open_sockets[i].fd);
    free(open_sockets[i].conf->addr);
    free(open_sockets[i].conf);
  }
  // for (int i = 0; i < MAX_OPEN_SOCKETS; i++) {
  //   free(drv->socket[i]->addr);
  //   free(drv->socket[i]);
  // }
  unload_all_plugins(drv->plugins);
  free(drv->socket);
  free(drv->routes_root);
  free(drv->server_root);
  free(drv);
  void *ptr;
  while ((ptr = pop_ptr())) {
    free(ptr);
  }
  pthread_mutex_destroy(&log_mutex);
  if (sigframe != -1)
    exit(0);
}
// Vim: set expandtab tabstop=2 shiftwidth=2:
