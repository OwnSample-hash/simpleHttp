#include "quit_handler.h"
#include <stdlib.h>

free_stack_t free_stack = {0};

void save_ptr(void *data) {
  if (free_stack.pos > STACK_COUNT) {
    log_error("Stack overflow dirty exit");
    free(data);
    quick_exit(1);
  }
  free_stack.ptrs[free_stack.pos++] = data;
}

void *pop_ptr() {
  if (free_stack.pos == 0) {
    return NULL;
  }
  return free_stack.ptrs[--free_stack.pos];
}
