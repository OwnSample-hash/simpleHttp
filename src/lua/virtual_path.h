#ifndef __HTTP__V_PATH__
#define __HTTP__V_PATH__

#include "setup.h"

/**
 * @enum lookup_status
 * @brief Status of the lookup
 */
typedef enum {
  OK_LOOKUP = 0,
  NIL,
  HANDLED,
  WRONG_VERB,
} lookup_status;

/**
 * @brief Resolve a virtual path
 *
 * @param path Path to resolve
 * @param cfd Client file descriptor
 * @param method HTTP method
 * @param keep_alive Keep alive settings
 * @return lookup_status Status of the lookup
 */
lookup_status virtual_path_resolv(const char *path, const int cfd,
                                  const char *method,
                                  const keep_alive_t *keep_alive);
#endif
