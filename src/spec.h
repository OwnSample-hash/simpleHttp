/**
 * @file
 * @brief HTTP response specification header file.
 */

#ifndef __HTTP_SPEC_H__
#define __HTTP_SPEC_H__

#include "itoa.h"
#include "write_response.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief A format string for the HTML payload.
 */
static const char payload_fmt[] = "<html>"
                                  "<head>"
                                  "</head>"
                                  "<body>"
                                  "<h1>%d</h1>"
                                  "<p>%s</p>"
                                  "</body>"
                                  "</html>";
/**
 * @brief A structure to hold the response specification.
 *
 * @var spec_response::status
 * @brief The HTTP status code
 *
 * @var spec_response::data
 * @brief  Either a short description about the status code or a file path
 *
 * @var spec_response::mode
 * @brief If null then data is treated as description or used for fopen(data,
 * mode)
 */
typedef struct spec_response {
  int status;
  char *data;
  char *mode;
} spec_response_t;
/**<@copydoc spec_response */

/**
 * @brief Generates a response based on the provided specification.
 *
 * @param res Pointer to the response structure.
 * @param spec The specification for the response, including status code,
 * data, and mode.
 * @return
 */
static inline size_t sr(response_t *res, spec_response_t spec) {
  set_status(res, spec.status);
  if (spec.mode == NULL) {
    size_t len = strlen(payload_fmt) + strlen(spec.data) +
                 strlen(TO_BASE(spec.status, 10));
    char *payload = calloc(len, sizeof(char));
    if (!payload) {
      perror("calloc,spec_response");
      return 0;
    }
    snprintf(payload, len, payload_fmt, spec.status, spec.data);
    set_payload(res, payload, len);
    add_header(res, "Content-Type", "text/html");
    add_header(res, "Content-Length", TO_BASE(len, 10));
    free(payload);
  } else {
    FILE *fp = fopen(spec.data, spec.mode);
    if (!fp) {
      perror("fopen,spec_response");
      return 0;
    }
    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *payload = calloc(len + 1, sizeof(char));
    if (!payload) {
      perror("calloc,spec_response");
      fclose(fp);
      return 0;
    }
    fread(payload, sizeof(char), len, fp);
    set_payload(res, payload, len);
    add_header(res, "Content-Type", "text/html");
    add_header(res, "Content-Length", TO_BASE(len, 10));
    fclose(fp);
    free(payload);
  }
  return write_response(res);
}
#endif // __HTTP_SPEC_H__
// Vim: set expandtab tabstop=2 shiftwidth=2:
