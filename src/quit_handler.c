#include "quit_handler.h"

void ctrl_c_h(int sigframe) {
  printf("\nQuitting and freeing things...%d\n", getpid());
  free(fds);
  exit(0);
}
