#ifndef __ITOA_HTTP__
#define __ITOA_HTTP__

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define ITOA_BASE_N (sizeof (unsigned) * CHAR_BIT + 2)
#define TO_BASE(x, b) itoa_base ((char[ITOA_BASE_N]){ 0 }, (x), (b))

char *itoa_base (char *s, int x, int base);

#endif // !__ITOA_HTTP__
