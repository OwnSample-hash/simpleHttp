/**
 * @file
 * @brief Public header file for the Simple HTTP Plugin System.
 */

#ifndef __HTTP_SIMPLE_HTTP_PLUGIN_H__
#define __HTTP_SIMPLE_HTTP_PLUGIN_H__

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Enumeration for plugin status codes.
 */
typedef enum {
  HTTP_PLUGIN_INVALID = -3,
  HTTP_PLUGIN_NOT_FOUND = -2,
  HTTP_PLUGIN_ERROR = -1,
  HTTP_PLUGIN_OK = 0,
} plugin_status_t;

/**
 * @struct plugin_info
 * @brief Structure representing plugin information.
 *
 * @var plugin_info::name
 * Name of the plugin.
 *
 * @var plugin_info::version
 * Version of the plugin.
 *
 * @var plugin_info::author
 * Author of the plugin.
 *
 * @var plugin_info::description
 * Description of the plugin.
 *
 * @var plugin_info::file
 * Path to the plugin file.
 *
 * @var plugin_info::flags
 * Different flags about the plugin
 */
typedef struct plugin_info {
  const char *name;
  const char *version;
  const char *author;
  const char *description;
  const char *file;
  enum {
    PLUGIN_FLAG_NONE = 0x00,     /**< No flags set */
    PLUGIN_FLAG_INTERNAL = 0x01, /**< Internal plugin type */
  } flags;
  int priority; /**< Priority of the plugin in events */
  bool enabled; /**< Whether the plugin is enabled or not */
} plugin_info_t;
/**<@copydoc plugin_info */

/**
 * @brief Initialize the plugin.
 *
 * @param info Pointer to the plugin_info_t structure containing plugin
 * information.
 * @return plugin_status_t Status of the initialization operation.
 */
extern plugin_status_t plugin_init(plugin_info_t *info);

/**
 * @brief Shutdown the plugin.
 * This function is called when the plugin is being unloaded.
 * @return
 */
extern plugin_status_t plugin_shutdown(plugin_info_t *info);

/**
 * @def PLUGIN_REGISTER
 * @brief Macro to register a plugin.
 * This macro initializes the plugin_info_t structure with the provided
 * information.
 *
 * @def PLUGIN_UNREGISTER
 * @brief Macro to unregister a plugin.
 *
 */

#define PLUGIN_REGISTER(info, pl_name, pl_version, pl_author, pl_description,  \
                        pl_flags, pl_priority)                                 \
  do {                                                                         \
    info->name = strdup(pl_name);                                              \
    info->version = strdup(pl_version);                                        \
    info->author = strdup(pl_author);                                          \
    info->description = strdup(pl_description);                                \
    info->flags = pl_flags;                                                    \
    info->priority = pl_priority;                                              \
  } while (0)

#define PLUGIN_UNREGISTER(info)                                                \
  do {                                                                         \
    free((void *)info->name);                                                  \
    free((void *)info->version);                                               \
    free((void *)info->author);                                                \
    free((void *)info->description);                                           \
    info->name = NULL;                                                         \
    info->version = NULL;                                                      \
    info->author = NULL;                                                       \
    info->description = NULL;                                                  \
  } while (0)

#endif // __HTTP_SIMPLE_HTTP_PLUGIN_H__
       // Vim: set expandtab tabstop=2 shiftwidth=2:
