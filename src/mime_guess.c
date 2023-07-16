#include "mime_guess.h"

char *ContentType(const char *fn) {
  const char *mime;
  magic_t magic;

  magic = magic_open(MAGIC_MIME_TYPE);
  magic_load(magic, NULL);
  mime = magic_file(magic, fn);
  int len = 17 + strlen(mime);
  char *ret = calloc(len, sizeof(char));
  if (ret == NULL) {
    perror("calloc");
  }
  snprintf(ret, len, "Content-Type: %s\r\n", mime);
  magic_close(magic);
  return ret;
}
