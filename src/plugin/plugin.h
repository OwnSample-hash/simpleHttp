/**
 * @file plugin.h
 * @brief Header file for the HTTP plugin system.
 *
 * This file defines the structures and functions used to manage HTTP plugins.
 * It includes definitions for plugin status, plugin information, and the
 * functions to initialize and load plugins.
 */
#ifndef __HTTP_PLUGIN_H__
#define __HTTP_PLUGIN_H__

#include "../link_list.h"
#include "simple_http_plugin.h"
#include <dlfcn.h>

/**
 * @struct http_plugin
 * @brief Structure representing a loaded HTTP plugin.
 *
 * @var http_plugin::info
 * Information about the plugin.
 *
 * @var http_plugin::handle
 * Handle to the loaded plugin.
 * This handle is used to access the plugin's functions and data.
 */
typedef struct http_plugin {
  plugin_info_t info;
  void *handle; // Handle to the loaded plugin
} http_plugin_t;
/**<@copydoc http_plugin*/

typedef node_t *plugin_node_pt; /**< Node type for linked list of plugins */

typedef plugin_status_t (*plugin_init_fn)(plugin_info_t *);
typedef plugin_init_fn plugin_shutdown_fn;

/**
 * @brief Initializes the plugin system.
 *
 * @param directory Directory where plugins are located.
 * @param plugins Pointer to an array of http_plugin_t pointers to be filled
 * with loaded plugins.
 * @param count Pointer to a size_t variable to store the number of loaded
 * plugins.
 * @return plugin_status_t Status of the initialization.
 */
plugin_status_t plugin_system_init(const char *directory,
                                   plugin_node_pt *plugins, size_t *count);

/**
 * @brief Loads a plugin by name.
 *
 * @param name Name of the plugin to load.
 * @return http_plugin_t* Pointer to the loaded plugin structure, or NULL if
 */
http_plugin_t *load_plugin(const char *name);

/**
 * @brief Unloads a plugin.
 *
 * @param plugin Pointer to the http_plugin_t structure representing the plugin
 * to unload.
 * @return plugin_status_t Status of the unload operation.
 */
plugin_status_t unload_plugin(plugin_node_pt plugin);

/**
 * @brief Cleans up the plugin system.
 *
 * This function should be called to free any resources allocated during plugin
 * initialization and loading.
 *
 * @param plugins Pointer to the array of loaded plugins.
 */
plugin_status_t unload_all_plugins(plugin_node_pt plugins);

/**
 * @brief Retrieves information about a plugin.
 *
 * @param plugin Pointer to the http_plugin_t structure representing the plugin.
 * @return const plugin_info_t* Pointer to the plugin information structure.
 */
const plugin_info_t *get_plugin_info(const http_plugin_t *plugin);

/**
 * @brief Triggers an event in the plugin system.
 * This function allows plugins to respond to specific events
 *
 * @param plugins Pointer to the array of loaded plugins.
 * @param event_name Name of the event to trigger.
 * @return plugin_status_t Status of the event trigger operation.
 */
plugin_status_t trigger_event(const http_plugin_t *plugins,
                              const char *event_name, ...);

/**
 *@brief Checks if a plugin is loaded.
 *
 * @param plugin Pointer to the http_plugin_t structure representing the plugin.
 * @return int Returns 1 if the plugin is loaded, 0 otherwise.
 */
int is_plugin_loaded(const http_plugin_t *plugin);
#endif // __HTTP_PLUGIN_H__
// Vim: set expandtab tabstop=2 shiftwidth=2:
