#include "mime_guess.h"
#include <signal.h>

char *ContentType(char *fn) {
  const char *mime;
  magic_t magic;

  magic = magic_open(MAGIC_MIME_TYPE);
  magic_load(magic, NULL);
  magic_compile(magic, NULL);
  mime = magic_file(magic, fn);
  raise(SIGTRAP);
  printf("Mime %s %s\n", fn, mime);
  magic_close(magic);
  return 0x0;
}
