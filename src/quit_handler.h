#ifndef __QUIT_HANDLER_HTTP__
#define __QUIT_HANDLER_HTTP__

#include <unistd.h>

#include "lua/setup.h"

/**
 * @brief The stack count
 */
#define STACK_COUNT 64

/**
 * @brief The stack to hold pointers to be freed
 */
typedef struct {
  unsigned int pos;
  void *ptrs[STACK_COUNT];
} free_stack_t;

/**
 * @brief The global stack
 */
extern free_stack_t free_stack;

/**
 * @brief Pop a pointer from the stack
 */
void *pop_ptr();
/**
 * @brief Push a pointer to the stack to be freed later
 *
 * @param data The pointer to be pushed
 */
void save_ptr(void *data);

/**
 * @brief The signal handler for SIGINT
 *
 * @param sigframe The signal frame
 */
void ctrl_c_h(int sigframe);
extern driver_t *drv;
#endif // !__QUIT_HANDLER_HTTP__
