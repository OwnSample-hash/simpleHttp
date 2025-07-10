/**
 * @file
 * @brief Virtual path resolution
 */

#ifndef __HTTP_V_PATH__
#define __HTTP_V_PATH__

#include "setup.h"

/**
 * @brief Enumeration for virtual path lookup status
 *
 * This enum defines the possible outcomes of a virtual path lookup.
 */
typedef enum {
  NIL = -1,
  OK_LOOKUP = 0,
  HANDLED,
  STATIC,
  WRONG_VERB,
} vlookup_status;

/**
 * @brief Resolve a virtual path
 *
 * @param path Path to resolve
 * @param cfd Client file descriptor
 * @param method HTTP method
 * @param keep_alive Keep alive settings
 * @return lookup_status Status of the lookup
 */
vlookup_status virtual_path_resolv(const char *path, const int cfd,
                                   const char *method,
                                   const keep_alive_t *keep_alive);

#endif
