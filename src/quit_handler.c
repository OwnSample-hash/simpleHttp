#include "quit_handler.h"

void ctrl_c_h(int sigframe) {
  printf("\nQuitting...\n");
  exit(0);
}
