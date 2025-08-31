#include "plugin.h"

#include "../log/log.h"
#include "simple_http_plugin.h"

plugin_status_t trigger_event(const plugin_node_pt plugins,
                              plugin_event_t event, ...) {
  if (plugins == NULL) {
    log_error("Plugin list is NULL");
    return HTTP_PLUGIN_ERROR;
  }
  node_t *current = plugins;
  while (current != NULL) {
    http_plugin_t *plugin = (http_plugin_t *)current->data;
    if (plugin == NULL) {
      log_error("Plugin is NULL");
      return HTTP_PLUGIN_ERROR;
    }
    if (!plugin->info.enabled) {
      current = current->next;
      continue;
    }
#define PL(ret, name, ...)                                                     \
  if (event == event_##name) {                                                 \
    if (plugin->info.events.name != NULL) {                                    \
      event_##name##_fn event_fn = plugin->info.events.name;                   \
      if (event_fn == NULL) {                                                  \
        log_warn("Plugin \"%s\" does not implement event \"%s\"",              \
                 (const char *)&(plugin->info), #name);                        \
        current = current->next;                                               \
        continue;                                                              \
      }                                                                        \
      va_list ap;                                                              \
      va_start(ap, event);                                                     \
      const char *args = #__VA_ARGS__;                                         \
      va_end(ap);                                                              \
    }                                                                          \
  }
    EVENTS
#undef PL

    current = current->next;
  }
  return HTTP_PLUGIN_OK;
}
// Vim: set expandtab tabstop=2 shiftwidth=2:
