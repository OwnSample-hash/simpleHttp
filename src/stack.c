#include "quit_handler.h"

free_stack_t free_stack;

void save_ptr(void *data) {
  if (free_stack.pos > STACK_COUNT) {
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
