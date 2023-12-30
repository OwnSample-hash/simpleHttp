#include "quit_handler.h"

void ctrl_c_h(int sigframe) {
  log_info("\nQuitting and freeing things... %d\n", getpid());
  lua_close(L);
  exit(0);
}
