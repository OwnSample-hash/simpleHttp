#include "parser.h"
#include "bytes.h"
#include "itoa.h"
#include "linkList.h"
#include "log/log.h"
#include "lua/setup.h"
#include "lua/virtual_path.h"
#include "mime_guess.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void erep(int client_fd) {
  static const char payload[] = "HTTP/1.1 404 Not Found\r\n"
                                "Connection: close\r\n"
                                "Content-Length: 65\r\n"
                                "Content-Type: text/html\r\n"
                                "\r\n"
                                "<html>\r\n"
                                "<head>\r\n"
                                "</head>\r\n"
                                "<body>\r\n"
                                "<h1>404</h1>\r\n"
                                "</body>\r\n"
                                "</html>\r\n";
  write(client_fd, payload, sizeof(payload));
}

splits_t splitOn_c(char *str, const char *delimiter) {
  splits_t head = NULL;
  char *token = strtok(str, delimiter);

  while (token != NULL) {
    insertNode(&head, token);
    token = strtok(NULL, delimiter);
  }

  return head;
}

parseGet_t parseReq(char *request, size_t srequest, int client_fd,
                    const char *root, const keep_alive_t *keep_alive) {
  log_info("Proccessing %ld bytes request", srequest);
  splits_t lines = splitOn_c(request, (const char *)"\n");
  char *cpyData = lines->data;
  splits_t wordsGet = splitOn_c(cpyData, (const char *)" ");
  // wordsGet[0] == GET
  // wordsGet[1] == $PATH
  // wordsGet[2] == HTTP/1.1
  if (wordsGet->next != NULL) {
    if (strcmp(wordsGet->next->data, "/") == 0)
      wordsGet->next->data = "/index.html";
  } else
    return 1;
  char *payload;
  int payload_len = 0;
  payload_len += strlen(wordsGet->data);
  payload_len += strlen(wordsGet->next->data);
  payload = calloc(payload_len + 2, sizeof(char));
  snprintf(payload, payload_len + 2, "%s %s", wordsGet->data,
           wordsGet->next->data);
  int ret = parseGet(payload, strlen(payload), client_fd, root, keep_alive);
  if (ret != OK_GET) {
    log_trace("praseGet(...) != OK_GET is true, ret: %d", ret);
    log_info("Treating path (%s) as a virtual path", wordsGet->next->data);
    ret = virtual_path_resolv(wordsGet->next->data, client_fd);
  }
  freeList(lines);
  freeList(wordsGet);
  free(payload);
  log_trace("Returing %d", ret);
  return ret;
}

parseGet_t parseGet(char *payload, size_t spayload, int client_fd,
                    const char *root, const keep_alive_t *keep_alive) {
  char *fn = calloc(spayload + 2, sizeof(char));
  if (fn == NULL) {
    log_fatal("CALLOC FAILED");
    perror("calloc");
    return CALLOC;
  }
  strncpy(fn, root, strlen(root));
  strncatskip(fn, payload, spayload, 4);
  log_info("Opening file: '%s' on root: '%s'", fn, root);
  FILE *fp = fopen(fn, "rb");
  if (fp == NULL) {
    return NOT_FOUND;
  }

  struct stat src_stat = {0};
  if (stat(fn, &src_stat)) {
    perror("stat");
    return NO_STAT;
  }
  char *cntTyp = ContentType(M_FILE, fn);
  int cntLen = strlen(cntTyp);
  char *str_size = TO_BASE(src_stat.st_size, 10);
  int len_strelen = strlen(str_size);
  // first param const int is 21 working should be 37
  char *header;
  int header_len = genHeader(&header, keep_alive);
  char *header_payload =
      calloc(37 + len_strelen + header_len + cntLen, sizeof(char));

  snprintf(header_payload, 37 + len_strelen + header_len + cntLen,
           "%s%s %jd\r\n%s", header, "Content-Length:", src_stat.st_size,
           cntTyp);

  write(client_fd, header_payload, strlen(header_payload));
  write(client_fd, "\r\n", 2);
  src_stat = (struct stat){0};

  char *buf = calloc(2 * MB_10, sizeof(char));
  if (buf == NULL) {
    perror("praseGet,calloc");
    log_fatal("prasGet,calloc");
    fclose(fp);
    free(fn);
    free(header_payload);
    free(cntTyp);
    free(header);
    return CALLOC;
  }
  size_t bytes_read = 0;
  while ((bytes_read = fread(buf, sizeof(char), 2 * MB_10, fp)) > 0) {
    write(client_fd, buf, bytes_read);
  }

  if (feof(fp))
    log_trace("eof hit on %s", fn);
  fclose(fp);
  free(buf);
  free(fn);
  free(header_payload);
  free(cntTyp);
  free(header);
  return OK_GET;
}

void strncatskip(char *dst, const char *src, size_t count, size_t offset) {
  size_t i, j;
  for (i = offset, j = strlen(dst); i < count; i++, j++) {
    dst[j] = src[i];
  }
  dst[j + 1] = '\0';
}

int genHeader(char **dst, const keep_alive_t *keep_alive) {
  if (keep_alive->keep_alive) {
    int len = strlen(HEADER_KEEP) + strlen(TO_BASE(keep_alive->timeout, 10)) +
              strlen(TO_BASE(keep_alive->max, 10));
    log_trace("Got %d len for keep alive header", len);
    *dst = calloc(len, sizeof(char));
    if (!dst) {
      log_fatal("Failed to calloc mem for header");
      perror("calloc");
      return -1;
    }
    return snprintf(*dst, len, HEADER_KEEP, keep_alive->timeout,
                    keep_alive->max);
  } else {
    *dst = calloc(strlen(HEADER_CLOSE), sizeof(char));
    if (!dst) {
      log_fatal("Failed to calloc mem for header");
      perror("calloc");
      return -1;
    }
    strncpy(*dst, HEADER_CLOSE, strlen(HEADER_CLOSE));
    return strlen(HEADER_CLOSE);
  }
}
