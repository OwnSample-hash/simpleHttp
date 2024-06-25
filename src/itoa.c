#include "itoa.h"
#include <stdlib.h>

char *itoa_base(char *s, int x, int base) {
  s += ITOA_BASE_N - 1;
  *s = '\0';
  if (base >= 2 && base <= 36) {
    int x0 = x;
    do {
      *(--s) = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[abs(x % base)];
      x /= base;
    } while (x);
    if (x0 < 0) {
      *(--s) = '-';
    }
  }
  return s;
}
