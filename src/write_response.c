/**
 * @file
 * @brief Response writing functions
 */

#include "write_response.h"
#include "itoa.h"
#include "log/log.h"
#include "mime_guess.h"
#include <errno.h>
#include <linux/limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/**
 * @struct response
 * @brief The respone struct opaque struct
 *
 * @var response::cfd
 * The client file descriptor
 *
 * @var response::additional_header
 * The additional headers
 *
 * @var response::header_count
 * The number of additional headers
 *
 * @var response::header_size
 * The size of the additional headers
 *
 * @var response::payload
 * The payload
 *
 * @var response::len
 * The length of the payload
 *
 * @var response::buffer
 * The buffer
 *
 * @var response::buffer_pos
 * The position in the buffer
 *
 * @var response::fp
 * The file pointer for the payload, if any
 */
struct response {
  const int cfd;
  const map_t *status;
  const keep_alive_t *keep_alive;
  header_t *additional_header;
  size_t header_count;
  size_t header_size;
  char *payload;
  size_t len;
  char *buffer;
  size_t buffer_pos;
  FILE *fp;
};

size_t write_response(response_t *res) {
  if (res == NULL) {
    log_error("Response is NULL");
    return -1;
  }
  if (res->cfd < 0) {
    log_error("Invalid file descriptor");
    return -1;
  }

  char *buf = NULL;
  res->buffer_pos = gen_header(&buf, res->status, res->keep_alive);
  strncpy(res->buffer, buf, res->buffer_pos);
  free(buf);
  log_info("Writing response to client %d", res->cfd);
  size_t total_write = 0;
  if (res->additional_header) {
    for (size_t i = 0; i < res->header_count; i++) {
      size_t tmp = snprintf((res->buffer + res->buffer_pos),
                            5 + strlen(res->additional_header[i].name) +
                                strlen(res->additional_header[i].value),
                            "%s: %s\r\n", res->additional_header[i].name,
                            res->additional_header[i].value);
      log_trace("name: \"%s\"", res->additional_header[i].name);
      log_trace("value: \"%s\"", res->additional_header[i].value);
      // tmp--;
      res->buffer_pos += tmp;
      total_write += tmp;
    }
  }
  log_debug("errno: %d", errno);
  size_t i = 0;
  while (i < res->buffer_pos) {
    size_t to_write =
        (res->buffer_pos - i < PIPE_BUF) ? (res->buffer_pos - i) : PIPE_BUF;
    size_t written = write(res->cfd, res->buffer + i, to_write);
    if (written < 0) {
      log_error("Failed to write header: %s", strerror(errno));
      return -1; // Error writing header
    }
    total_write += written;
    i += to_write;
  }

  log_debug("errno: %d", errno);
  perror("write header");
  if (!res->payload) {
    return total_write;
  }

  size_t written = write(res->cfd, HTTP_EMPTY_LINE, HTTP_EMPTY_LINE_LEN);
  if (written < 0) {
    log_error("Failed to write empty line: %s", strerror(errno));
    return -1; // Error writing empty line
  }
  total_write += written;

  i = 0;
  while (i < res->len) {
    size_t to_write = (res->len - i < PIPE_BUF) ? (res->len - i) : PIPE_BUF;
    size_t written = write(res->cfd, res->payload + i, to_write);
    if (written < 0) {
      log_error("Failed to write payload: %s", strerror(errno));
      return -1; // Error writing payload
    }
    total_write += written;
    i += to_write;
  }

  log_debug("errno: %d", errno);
  return total_write;
}

response_t *new_response(int cfd, const map_t *status,
                         const keep_alive_t *keep_alive) {
  response_t *res = calloc(1, sizeof(response_t));
  if (!res) {
    log_fatal("Failed to calloc mem for response");
    return NULL;
  }
  int *ccfd = (int *)&res->cfd;
  *ccfd = cfd;
  res->additional_header = NULL;
  res->header_count = 0;
  res->header_size = 0;
  res->payload = NULL;
  res->fp = NULL;
  res->len = 0;
  res->buffer = calloc(DEFAULT_LEN, sizeof(char));
  if (!res->buffer) {
    log_fatal("Failed to calloc mem for buffer");
    return NULL;
  }
  res->buffer_pos = 0;
  res->status = status ? status : &STATUS_MAP[0]; // Default to 200 OK
  res->keep_alive =
      keep_alive ? keep_alive
                 : &(keep_alive_t){.keep_alive = 0, .timeout = 0, .max = 0};
  return res;
}

header_t *expandHeader(header_t *header, size_t size) {
  header_t *tmp = realloc(header, (1 << size) * sizeof(header_t));
  if (!tmp) {
    log_fatal("Failed to realloc mem for header");
    return NULL;
  }
  return tmp;
}

void add_header(response_t *res, const char *name, const char *value) {
  if (res->additional_header == NULL) {
    res->additional_header = calloc(1, sizeof(header_t));
    if (!res->additional_header) {
      log_fatal("Failed to calloc mem for header");
      return;
    }
  }
  if (res->header_count == 0 || res->header_count == (1 << res->header_size)) {
    res->additional_header =
        expandHeader(res->additional_header, ++(res->header_size));
  }
  res->additional_header[res->header_count].name =
      calloc(strlen(name), sizeof(char));
  res->additional_header[res->header_count].value =
      calloc(strlen(value), sizeof(char));
  if (!res->additional_header[res->header_count].name ||
      !res->additional_header[res->header_count].value) {
    log_fatal("Failed to calloc mem for header");
    return;
  }

  strncpy((char *)res->additional_header[res->header_count].name, name,
          strlen(name));
  strncpy((char *)res->additional_header[res->header_count].value, value,
          strlen(value));
  res->header_count++;
}

void trim_response(response_t *res) {
  log_trace("Trimming header from %d to %d", 1 << res->header_size,
            res->header_count);
  header_t *tmp =
      realloc(res->additional_header, res->header_count * sizeof(header_t));
  if (!tmp) {
    log_fatal("Failed to realloc mem for header");
  }
  res->additional_header = tmp;
}

int gen_header(char **dst, const map_t *status,
               const keep_alive_t *keep_alive) {
  size_t len = strlen(status->key.p2) + strlen(status->value);
  if (keep_alive->keep_alive) {
    len += strlen(HEADER_KEEP) + strlen(TO_BASE(keep_alive->timeout, 10)) +
           strlen(TO_BASE(keep_alive->max, 10));
    log_trace("Got %d len for keep alive header", len);
    *dst = calloc(len, sizeof(char));
    if (!dst) {
      log_fatal("Failed to calloc mem for header");
      perror("calloc");
      return -1;
    }
    return snprintf(*dst, len, HEADER_KEEP, status->key.p1, status->value,
                    keep_alive->timeout, keep_alive->max);
  } else {
    len += strlen(HEADER_CLOSE);
    *dst = calloc(len, sizeof(char));
    if (!dst) {
      log_fatal("Failed to calloc mem for header");
      perror("calloc");
      return -1;
    }
    return snprintf(*dst, len, HEADER_CLOSE, status->key.p1, status->value);
  }
}

void free_response(response_t *resp) {
  if (resp->additional_header != NULL) {
    for (size_t i = 0; i < resp->header_count; i++) {
      free((char *)resp->additional_header[i].name);
      free((char *)resp->additional_header[i].value);
    }
    free(resp->additional_header);
  }
  free(resp->buffer);
  free(resp->payload);
  free(resp);
}

size_t set_payload(response_t *res, const char *payload, size_t len) {
  log_trace("Setting payload of len %d", len);
  res->payload = calloc(len, sizeof(char));
  if (!res->payload) {
    log_fatal("Failed to calloc mem for payload");
    return -1;
  }
  memcpy(res->payload, payload, len);
  res->len = len;
  return len;
}

size_t add_to_Payload(response_t *res, const char *payload, size_t len) {
  log_trace("Adding payload of len %d", len);
  if (res->payload == NULL) {
    return set_payload(res, payload, len);
  }
  char *tmp = realloc(res->payload, res->len + len);
  if (!tmp) {
    log_fatal("Failed to realloc mem for payload");
    return -1;
  }
  res->payload = tmp;
  memcpy(res->payload + res->len, payload, len);
  res->len += len;
  return res->len;
}

void set_status(response_t *res, int code) {
  const map_t *status = lookup_status(code);
  if (status) {
    res->status = status;
  } else {
    log_error("Unknown status code: %d", code);
    res->status = &STATUS_MAP[0]; // Default to 200 OK
  }
}

size_t add_file_to_payload(response_t *res, const char *file,
                           const char *mode) {
  if (res->fp) {
    log_error("File pointer already set");
    return -1;
  }
  res->fp = fopen(file, mode);
  if (!res->fp) {
    log_error("Failed to open file %s: %s", file, strerror(errno));
    return -1;
  }
  fseek(res->fp, 0, SEEK_END);
  res->len = ftell(res->fp);
  fseek(res->fp, 0, SEEK_SET);
  res->payload = malloc(res->len);
  if (!res->payload) {
    log_fatal("Failed to malloc mem for payload");
    fclose(res->fp);
    res->fp = NULL;
    return -1;
  }
  fread(res->payload, sizeof(char), res->len, res->fp);
  fclose(res->fp);
  res->fp = NULL;
  char *content_type = ContentType(M_FILE, file);
  if (!content_type) {
    log_error("Failed to get content type for file %s", file);
    free(res->payload);
    res->payload = NULL;
    res->len = 0;
    return -1;
  }
  add_header(res, "Content-Type", content_type);
  free(content_type);
  add_header(res, "Content-Length",
             TO_BASE(res->len, 10)); // Add content length header
  return res->len;
}

bool is_valid(const response_t *res) {
  if (res == NULL) {
    log_error("Response is NULL");
    return false;
  }
  if (res->cfd < 0) {
    log_error("Invalid file descriptor");
    return false;
  }
  if (res->status == NULL) {
    log_error("Status is NULL");
    return false;
  }
  if (res->buffer == NULL || res->buffer_pos == 0) {
    log_error("Buffer is NULL or empty");
    return false;
  }
  if (res->payload == NULL && res->len > 0) {
    log_error("Payload is NULL but length is greater than zero");
    return false;
  }
  return true;
}

const map_t *lookup_status(int code) {
  for (int i = 0; STATUS_MAP[i].key.p1 != 0; i++) {
    if (STATUS_MAP[i].key.p1 == code) {
      return &STATUS_MAP[i];
    }
  }
  return NULL;
}
// Vim: set expandtab tabstop=2 shiftwidth=2:
