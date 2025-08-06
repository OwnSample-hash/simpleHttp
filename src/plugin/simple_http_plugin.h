/**
 * @file
 * @brief Public header file for the Simple HTTP Plugin System.
 */

#ifndef __HTTP_SIMPLE_HTTP_PLUGIN_H__
#define __HTTP_SIMPLE_HTTP_PLUGIN_H__

#ifndef _PLUGIN_
#include "../log/log.h"
#else
extern log_log_fn log_log;
#endif
#include <dlfcn.h>
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
 * @brief Enumeration for plugin events.
 * This enumeration defines the events that plugins can listen to.
 * The values are used as indices in the plugin_events_t structure.
 */
typedef enum plugin_event {
  EVENT_PRE_ON_INCOMING_CLIENT = sizeof(void *) * 0,
  EVENT_POST_ON_INCOMING_CLIENT = sizeof(void *) * 1,
} plugin_event_t;

#define EVENTS                                                                 \
  PL(plugin_status_t, event_pre_on_incoming_client, int client_fd)             \
  PL(plugin_status_t, event_post_on_incoming_client, int client_fd)
// EVENTS ENDS

#define PL(ret, name, ...) typedef ret (*name##_fn)(__VA_ARGS__);
EVENTS
#undef PL

typedef struct {
#define PL(ret, name, ...) name##_fn name;
  EVENTS
#undef PL
} plugin_events_t;

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
  int priority;           /**< Priority of the plugin in events */
  bool enabled;           /**< Whether the plugin is enabled or not */
  log_log_fn log_log;     /**< Pointer to the logging function */
  plugin_events_t events; /**< Events associated with the plugin */
} plugin_info_t;
/**<@copydoc plugin_info */

static const plugin_info_t *plugin_info =
    NULL; /**< Pointer to the plugin information structure */

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

// log_log = info->log_log;
// log_log = dlsym(RTLD_DEFAULT, "log_log");                                  \
// if (log_log == NULL) {                                                     \
//   fprintf(stderr, "Failed to find log_log function: %s", dlerror());       \
//   return HTTP_PLUGIN_ERROR;                                                \
// }
#define PLUGIN_REGISTER(info, pl_name, pl_version, pl_author, pl_description,  \
                        pl_flags, pl_priority)                                 \
  do {                                                                         \
    plugin_info = info;                                                        \
    info->name = strdup(pl_name);                                              \
    info->version = strdup(pl_version);                                        \
    info->author = strdup(pl_author);                                          \
    info->description = strdup(pl_description);                                \
    info->flags = pl_flags;                                                    \
    info->priority = pl_priority;                                              \
    if (info->name == NULL || info->version == NULL || info->author == NULL || \
        info->description == NULL) {                                           \
      log_error("Failed to allocate memory for plugin information");           \
      return HTTP_PLUGIN_ERROR;                                                \
    }                                                                          \
    info->enabled = true;                                                      \
    log_info("Plugin \"%s\" registered successfully", info->name);             \
  } while (0)
// memset(&info->events, 0, sizeof(plugin_events_t));

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
