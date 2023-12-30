#ifndef __MIME_GUESS_HTTP__
#define __MIME_GUESS_HTTP__

#include <magic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { M_FILE, M_BUFFER } m_type;

char *ContentType(m_type type, const char *payload);

#endif // !__MIME_GUESS_HTTP__
