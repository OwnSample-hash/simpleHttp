changecom(`/*', `*/')
#ifndef HEADER_GUARD
#define HEADER_GUARD

#include "../itoa.h"

static const char SC_payload[] = "<html>\r\n"
                                           "<head>\r\n"
                                           "</head>\r\n"
                                           "<body>\r\n"
                                           "<h1>SC</h1>\r\n"
                                           "<p>DESC</p>\r\n"
                                           "</body>\r\n"
                                           "</html>\r\n";

static const char *HTTP_SC_PAGE;

#define SR_SC(res, cfd)                                                        \
  if (!HTTP_SC_PAGE) {                                                         \
    set_payload(res, SC_payload, sizeof(SC_payload) - 1);                      \
    add_header(res, "Content-Type", "text/html");                              \
    add_header(res, "Content-Length", TO_BASE(sizeof(SC_payload) - 1, 10));    \
    set_status(res, SC);                                                       \
  } else {                                                                     \
    FILE *fp = fopen(HTTP_SC_PAGE, "r");                                       \
    if (fp) {                                                                  \
      fseek(fp, 0, SEEK_END);                                                  \
      size_t size = ftell(fp);                                                 \
      fseek(fp, 0, SEEK_SET);                                                  \
      char *buffer = malloc(size + 1);                                         \
      if (buffer) {                                                            \
        fread(buffer, 1, size, fp);                                            \
        buffer[size] = '\0';                                                   \
        set_payload(res, buffer, size);                                        \
        free(buffer);                                                          \
      }                                                                        \
      add_header(res, "Content-Type", "text/html");                            \
      add_header(res, "Content-Length", TO_BASE(size, 10));                    \
      set_status(res, SC);                                                     \
      fclose(fp);                                                              \
    } else {                                                                   \
      log_error("Failed to open SC page file: %s",                             \
                HTTP_SC_PAGE);                                                 \
      set_payload(res, SC_payload, sizeof(SC_payload) - 1);                    \
    }                                                                          \
  }                                                                            \
  log_trace("Wrote %ju byte to client %d", write_response(res), cfd);

#endif
