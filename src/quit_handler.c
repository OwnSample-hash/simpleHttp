#include "quit_handler.h"

void ctrl_c_h(int sigframe) {
  printf("\nQuitting...%d\n", getpid());
  exit(0);
}
