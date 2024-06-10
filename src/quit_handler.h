#ifndef __QUIT_HANDLER_HTTP__
#define __QUIT_HANDLER_HTTP__

#include <stdlib.h>
#include <unistd.h>

#include "lua/setup.h"

#define STACK_COUNT 64

typedef struct {
  unsigned int pos;
  void *ptrs[STACK_COUNT];
} free_stack_t;

extern free_stack_t free_stack;

void *pop_ptr();
void save_ptr(void *data);

void ctrl_c_h(int sigframe);
extern driver *drv;
#endif // !__QUIT_HANDLER_HTTP__
