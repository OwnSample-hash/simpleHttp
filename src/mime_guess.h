/**
 * @file
 * @brief MIME guessing
 */

#ifndef __MIME_GUESS_HTTP__
#define __MIME_GUESS_HTTP__

#include <magic.h>
#include <stddef.h>

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

typedef struct {
  const char *name; /**< Name of the MIME type */
  const char *ext;  /**< File extension associated with the MIME type */
} mime_type_t;

static mime_type_t mime_map[] = {
    {"text/html", ".html"},
    {"text/css", ".css"},
    {"application/javascript", ".js"},
    {"application/json", ".json"},
    {"application/xml", ".xml"},
    {NULL, NULL}, // Sentinel value to mark the end of the array
};

#endif // !__MIME_GUESS_HTTP__
// Vim: set expandtab tabstop=2 shiftwidth=2:
