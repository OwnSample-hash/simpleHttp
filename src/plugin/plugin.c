#define _XOPEN_SOURCE 500
#define _GNU_SOURCE
#include "plugin.h"
#include "../log/log.h"
#include <fnmatch.h>
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct stat stat_t;
typedef struct FTW FTW_t;

size_t count_ = 0;
plugin_node_pt plugins_ = NULL;

int filter(const char *path, const stat_t *sb, int tflag, FTW_t *tfwbuf) {
  if (tflag == FTW_D && tfwbuf->level > 0)
    return FTW_SKIP_SUBTREE;
  if (tflag != FTW_F) {
    log_trace("Ignoring \"%s\" on level %d", path, tfwbuf->level);
    return FTW_CONTINUE;
  }
  if (fnmatch("*.so", path, FNM_NOESCAPE) != 0) {
    log_trace("Ignoring non-plugin file: %s", path);
    return FTW_CONTINUE;
  }
  log_trace("Found plugin file: %s with size %ld", path, sb->st_size);
  http_plugin_t *plugin = calloc(sizeof(http_plugin_t), 1);
  if (!plugin) {
    log_error("Failed to allocate memory for plugin");
    perror("calloc");
    return FTW_STOP;
  }
  plugin->info.file = strdup(path);
  if (!plugin->info.file) {
    log_error("Failed to duplicate plugin file path: %s", path);
    free(plugin);
    return FTW_STOP;
  }
  insert_node(&plugins_, plugin, sizeof(http_plugin_t));
  count_++;
  free(plugin);
  return FTW_CONTINUE;
}

plugin_status_t plugin_system_init(const char *directory,
                                   plugin_node_pt *plugins, size_t *count) {
  if (directory == NULL || strlen(directory) == 0) {
    log_error("Invalid directory provided for plugin initialization");
    return HTTP_PLUGIN_INVALID;
  }
  if (count == NULL) {
    log_error("Count pointer is NULL, cannot return number of plugins");
    return HTTP_PLUGIN_INVALID;
  }

  if (nftw(directory, filter, 5, FTW_PHYS | FTW_ACTIONRETVAL)) {
    log_error("nftw failed to scan directory: %s", directory);
    perror("nftw");
    return HTTP_PLUGIN_ERROR;
  }
  *count = count_;
  log_info("Found %zu plugins in directory: %s", *count, directory);
  if (plugins != NULL) {
    *plugins = plugins_;
  } else {
    log_error("Plugins pointer is NULL, cannot return loaded plugins");
    return HTTP_PLUGIN_INVALID;
  }

  if (*count == 0) {
    log_info("No plugins found in directory: %s", directory);
    return HTTP_PLUGIN_NOT_FOUND;
  }

  plugin_node_pt current = *plugins;
  while (current != NULL) {
    http_plugin_t *plugin = (http_plugin_t *)current->data;
    if (plugin == NULL || plugin->info.file == NULL) {
      log_error("Invalid plugin data found in the list");
      continue;
    }
    plugin->handle = dlopen(plugin->info.file, RTLD_NOW | RTLD_LOCAL);
    if (plugin->handle == NULL) {
      log_error("Failed to load plugin %s: %s", plugin->info.file, dlerror());
      continue;
    }
    plugin_init_fn init_fn =
        (plugin_init_fn)dlsym(plugin->handle, "plugin_init");
    if (init_fn == NULL) {
      log_error("Failed to find plugin_init function in %s: %s",
                plugin->info.file, dlerror());
      dlclose(plugin->handle);
      continue;
    }
    plugin_status_t status = init_fn(&plugin->info);
    if (status != HTTP_PLUGIN_OK) {
      log_error("Plugin %s initialization failed with status: %d",
                plugin->info.file, status);
      dlclose(plugin->handle);
      continue;
    }
    log_info("Plugin found: \"%s\" at %s", plugin->info.name,
             plugin->info.file);
    current = current->next;
  }

  return HTTP_PLUGIN_OK;
}

void free_plugin(void *data) {
  if (data == NULL)
    return;
  http_plugin_t *plugin = (http_plugin_t *)data;
  if (plugin->handle != NULL) {
    plugin_shutdown_fn shutdown_fn =
        (plugin_shutdown_fn)dlsym(plugin->handle, "plugin_shutdown");
    if (shutdown_fn == NULL) {
      log_error("Failed to find plugin_shutdown function: %s", dlerror());
    }
    plugin_status_t status = shutdown_fn(&plugin->info);
    if (status != HTTP_PLUGIN_OK) {
      log_error("Plugin shutdown failed with status: %d", status);
    }

    dlclose(plugin->handle);
  }
  if (plugin->info.file != NULL) {
    free((void *)plugin->info.file);
  }
  free(plugin);
}

plugin_status_t unload_all_plugins(plugin_node_pt plugins) {
  if (plugins == NULL) {
    log_error("No plugins to unload");
    return HTTP_PLUGIN_INVALID;
  }
  free_list(plugins, free_plugin);
  return HTTP_PLUGIN_OK;
}
// Vim: set expandtab tabstop=2 shiftwidth=2:
