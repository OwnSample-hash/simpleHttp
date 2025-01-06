#include "write_response.h"
#include "itoa.h"
#include "log/log.h"
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

struct response {
  int cfd;
  header_t *additional_header;
  size_t header_count;
  size_t header_size;
  char *payload;
  size_t len;
  char *buffer;
  size_t buffer_pos;
};

size_t writeResponse(response_t *res) {
  if (res == NULL) {
    log_error("Response is NULL");
    return -1;
  }
  if (res->cfd < 0) {
    log_error("Invalid file descriptor");
    return -1;
  }
  log_info("Writing response to client %d", res->cfd);
  size_t total_write = 0;
  if (res->additional_header) {
    for (size_t i = 0; i < res->header_count; i++) {
      size_t tmp = snprintf((res->buffer + res->buffer_pos),
                            6 + strlen(res->additional_header[i].name) +
                                strlen(res->additional_header[i].value),
                            "%s: %s\r\n", res->additional_header[i].name,
                            res->additional_header[i].value);
      log_trace("name: %s", res->additional_header[i].name);
      log_trace("value: %s", res->additional_header[i].value);
      res->buffer_pos += tmp;
      total_write += tmp;
    }
  }
  size_t i = 0;
  while (i < res->buffer_pos) {
    size_t to_write =
        (res->buffer_pos - i < PIPE_BUF) ? (res->buffer_pos - i) : PIPE_BUF;
    total_write += write(res->cfd, res->buffer + i, to_write);
    i += to_write;
  }

  if (!res->payload) {
    return total_write;
  }

  total_write += write(res->cfd, HTTP_EMPTY_LINE, HTTP_EMPTY_LINE_LEN);

  i = 0;
  while (i < res->len) {
    size_t to_write = (res->len - i < PIPE_BUF) ? (res->len - i) : PIPE_BUF;
    total_write += write(res->cfd, res->payload + i, to_write);
    i += to_write;
  }

  return total_write;
}

[[nodiscard("You have to free it later")]] response_t *
newRespones(int cfd, const map_t *status, const keep_alive_t *keep_alive) {
  response_t *res = calloc(1, sizeof(response_t));
  if (!res) {
    log_fatal("Failed to calloc mem for response");
    return NULL;
  }
  res->cfd = cfd;
  res->additional_header = NULL;
  res->header_count = 0;
  res->header_size = 0;
  res->payload = NULL;
  res->len = 0;
  res->buffer = calloc(DEFAULT_LEN, sizeof(char));
  if (!res->buffer) {
    log_fatal("Failed to calloc mem for buffer");
    return NULL;
  }
  char *buf = NULL;
  res->buffer_pos = genHeader(&buf, status, keep_alive);
  snprintf(res->buffer, res->buffer_pos + 1, "%s", buf);
  free(buf);
  /*res->buffer_pos++;*/
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

void addHeader2Response(response_t *res, const char *name, const char *value) {
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
      calloc(strlen(name) + 1, sizeof(char));
  res->additional_header[res->header_count].value =
      calloc(strlen(value) + 1, sizeof(char));
  if (!res->additional_header[res->header_count].name ||
      !res->additional_header[res->header_count].value) {
    log_fatal("Failed to calloc mem for header");
    return;
  }
  snprintf((char *)res->additional_header[res->header_count].name,
           strlen(name) + 1, "%s", name);
  snprintf((char *)res->additional_header[res->header_count].value,
           strlen(value) + 1, "%s", value);

  /*strncpy((char *)res->additional_header[res->header_count].name, name,*/
  /*        strlen(name));*/
  /*strncpy((char *)res->additional_header[res->header_count].value, value,*/
  /*        strlen(value));*/
  res->header_count++;
}

void trimResponse(response_t *res) {
  log_trace("Trimming header from %d to %d", 1 << res->header_size,
            res->header_count);
  header_t *tmp =
      realloc(res->additional_header, res->header_count * sizeof(header_t));
  if (!tmp) {
    log_fatal("Failed to realloc mem for header");
  }
  res->additional_header = tmp;
}

int genHeader(char **dst, const map_t *status, const keep_alive_t *keep_alive) {
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

void freeResponse(response_t *resp) {
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

size_t setPayload(response_t *res, const char *payload, size_t len) {
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

const map_t *lookupStatus(int code) {
  for (int i = 0; STATUS_MAP[i].key.p1 != 0; i++) {
    if (STATUS_MAP[i].key.p1 == code) {
      return &STATUS_MAP[i];
    }
  }
  return NULL;
}
