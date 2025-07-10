#include "mime_guess.h"
#include "log/log.h"
#include <stdlib.h>
#include <string.h>

char *ContentType(m_type type, const char *payload) {
  if (payload == NULL) {
    log_error("Payload is NULL");
    return NULL;
  }
  const char *dot = strrchr(payload, '.');
  if (!dot)
    goto cont;
  log_trace("File extension: %s", dot);
  for (int i = 0; mime_map[i].name != NULL; i++) {
    if (strcmp(mime_map[i].ext, dot) == 0) {
      log_trace("Found MIME type: %s for extension: %s", mime_map[i].name,
                mime_map[i].ext);
      return strdup(mime_map[i].name);
    }
  }

  // If no match found, use libmagic to guess the MIME type
cont:
  __asm__ __volatile__("nop" : : : "memory");
  const char *mime;
  magic_t magic;
  magic = magic_open(MAGIC_MIME | MAGIC_SYMLINK | MAGIC_CHECK);
  if (magic == NULL) {
    log_error("No magic");
    perror("magic");
  }
  magic_load(magic, NULL);
  switch (type) {
  case M_FILE:
    log_trace("M_FILE for %s", payload);
    mime = magic_file(magic, payload);
    break;
  case M_BUFFER:
    log_trace("M_BUFFER for __suppressd__");
    mime = magic_buffer(magic, payload, strlen(payload));
    break;
  default:
    log_error("Default case no vaild magic type supplied");
    return NULL;
    break;
  }
  int len = strlen(mime);
  char *ret = calloc(len, sizeof(char));
  if (ret == NULL) {
    perror("calloc");
  }
  snprintf(ret, len, "%s", mime);
  log_trace("Mime type: %s", mime);
  magic_close(magic);
  return ret;
}
// Vim: set expandtab tabstop=2 shiftwidth=2:
