#define _PLUGIN_ 1
#include "../../../src/log/log.h"
#include "simple_http_plugin.h"

plugin_status_t e_pre_on_incoming_client(int client) {
  log_info("Pre-processing incoming client: %d", client);
  return HTTP_PLUGIN_OK;
}

plugin_status_t e_post_on_incoming_client(int client) {
  log_info("Post-processing incoming client: %d", client);
  return HTTP_PLUGIN_OK;
}

plugin_status_t plugin_init(plugin_info_t *info) {
  if (info == NULL) {
    return HTTP_PLUGIN_INVALID;
  }
  info->log_log(0, __FILE__, __LINE__, "Initializing Simple HTTP Plugin");
  info->events.pre_on_incoming_client = e_pre_on_incoming_client;
  info->events.post_on_incoming_client = e_post_on_incoming_client;
  info->log_log(0, __FILE__, __LINE__, "Initializing Simple HTTP Plugin");
  // Initialize the plugin information
  PLUGIN_REGISTER(info, "Simple HTTP Plugin", "1.0.0", "Author Name",
                  "A simple HTTP plugin example", PLUGIN_FLAG_NONE, 0);
  // Plugin initialization logic can go here
  log_info("Plugin \"%s\" initialized successfully", info->name);

  return HTTP_PLUGIN_OK;
}

plugin_status_t plugin_shutdown(plugin_info_t *info) {
  // Perform any necessary cleanup before the plugin is unloaded
  // This could include freeing resources, closing connections, etc.
  if (info == NULL) {
    return HTTP_PLUGIN_INVALID;
  }
  PLUGIN_UNREGISTER(info);

  return HTTP_PLUGIN_OK;
}

// Vim: set expandtab tabstop=2 shiftwidth=2:
