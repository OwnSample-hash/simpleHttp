/**
 * @file
 * @brief Quit handler
 */

#ifndef __QUIT_HANDLER_HTTP__
#define __QUIT_HANDLER_HTTP__

#include <unistd.h>

#include "config.h"
#include "lua/setup.h"

/**
 * @struct free_stack
 * @brief The stack to hold pointers to be freed when quitting
 *
 * @var free_stack::pos
 * The position and size of the stack
 *
 * @var free_stack::ptrs
 * Pointers to be freed
 */
typedef struct free_stack {
  unsigned int pos;
  void *ptrs[STACK_COUNT];
} free_stack_t;
/**<@copydoc free_stack*/

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

/**
 * @brief Global driver
 */
extern driver_t *drv;
#endif // !__QUIT_HANDLER_HTTP__
