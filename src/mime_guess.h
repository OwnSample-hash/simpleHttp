#ifndef __MIME_GUESS_HTTP__
#define __MIME_GUESS_HTTP__

#include <magic.h>

/**
 * @brief The type of the magic
 */
typedef enum {
  M_FILE,
  M_BUFFER,
} m_type;

/**
 * @brief Get the Content Type object
 *
 * @param type The type of the magic
 * @param payload The payload to check
 * @return char* The content type
 */
char *ContentType(m_type type, const char *payload);

#endif // !__MIME_GUESS_HTTP__
