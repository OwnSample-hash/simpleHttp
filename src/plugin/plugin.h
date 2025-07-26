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

/**
 * @class plugin_search_info
 * @brief Structure for searching plugins.
 * This structure is used to specify the way for searching plugins
 * by name, path, or index.
 *
 * @var plugin_search_info::data
 * Pointer to the data used for searching.
 * This can be a string for name/path searches or an size_t for index searches.
 *
 */
typedef struct plugin_search_info {
  enum {
    PLUGIN_SEARCH_BY_NAME = 0,
    PLUGIN_SEARCH_BY_PATH,
    PLUGIN_SEARCH_BY_IDX
  } type; /**< Type of search: by name, path or index  */
  const void *data;
} plugin_search_info_t;
/**<@copydoc plugin_search_info*/

typedef node_t *plugin_node_pt; /**< Node type for linked list of plugins */

typedef plugin_status_t (*plugin_init_fn)(
    plugin_info_t *); /**< Function type for plugin initialization */
typedef plugin_init_fn
    plugin_shutdown_fn; /**< Function type for plugin shutdown */

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
 * @param plugins Pointer to the linked list of loaded plugins.
 * @param path Path of the plugin to load.
 * @return plugin_status_t Status of the load operation.
 */
plugin_status_t load_plugin(plugin_node_pt plugins, const char *path);

/**
 * @brief Unloads a plugin.
 *
 * @param plugin Pointer to the http_plugin_t structure representing the plugin
 * to unload.
 * @param search_info Pointer to the plugin_search_info_t structure
 * @return plugin_status_t Status of the unload operation.
 */
plugin_status_t unload_plugin(plugin_node_pt plugin,
                              const plugin_search_info_t *search_info);

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
 * @param search_info Pointer to the plugin_search_info_t structure containing
 * @return const plugin_info_t* Pointer to the plugin information structure.
 */
const plugin_info_t *get_plugin_info(const plugin_node_pt plugin,
                                     const plugin_search_info_t *search_info);

/**
 * @brief Triggers an event in the plugin system.
 * This function allows plugins to respond to specific events
 *
 * @param plugins Pointer to the array of loaded plugins.
 * @param event_name Name of the event to trigger.
 * @return plugin_status_t Status of the event trigger operation.
 */
plugin_status_t trigger_event(const plugin_node_pt plugins,
                              const char *event_name, ...);

/**
 *@brief Checks if a plugin is loaded.
 *
 * @param plugin Pointer to the http_plugin_t structure representing the plugin.
 * @param search_info Pointer to the plugin_search_info_t structure containing
 * @return int Returns 1 if the plugin is loaded, 0 otherwise.
 * When searching by index, on failure, it returns the size of the plugins list.
 */
int is_plugin_loaded(const plugin_node_pt plugin,
                     const plugin_search_info_t *search_info);
#endif // __HTTP_PLUGIN_H__
// Vim: set expandtab tabstop=2 shiftwidth=2:
