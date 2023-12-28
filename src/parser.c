#include "parser.h"
#include "bytes.h"
#include "itoa.h"
#include "linkList.h"
#include "mime_guess.h"

void erep(int client_fd) {
  static const char payload[] = "HTTP/1.1 404 Not Found\r\n"
                                "Connection: close\r\n"
                                "Content-Length: 90\r\n"
                                "Content-Type: text/html\r\n"
                                "\r\n"
                                "<html>\r\n"
                                "<head>\r\n"
                                "</head>\r\n"
                                "<body>\r\n"
                                "  <h1>404</h1>\r\n"
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

int parseReq(char *request, size_t srequest, int client_fd) {
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
  freeList(lines);
  freeList(wordsGet);
  int ret = parseGet(payload, strlen(payload), client_fd);
  free(payload);
  return ret;
}

int parseGet(char *payload, size_t spayload, int client_fd) {
  char *fn = calloc(spayload + 1, sizeof(char));
  strncpy(fn, "./server", 8);
  strncatskip(fn, payload, spayload, 4);
  log_info("Opening file: '%s'", fn);
  FILE *fp = fopen(fn, "rb");
  if (fp == NULL) {
    perror("fopen");
    return 1;
  }

  const char HEADER[] = "HTTP/1.1 200 Ok\r\nConnection: close\r\n";
  struct stat src_stat;
  if (stat(fn, &src_stat)) {
    perror("stat");
    return 1;
  }
  char *cntTyp = ContentType(fn);
  int cntLen = strlen(cntTyp);
  char *str_size = TO_BASE(src_stat.st_size, 10);
  int len_strelen = strlen(str_size);
  char *header_payload =
      calloc(21 + len_strelen + strlen(HEADER) + cntLen, sizeof(char));
  snprintf(header_payload, 21 + len_strelen + strlen(HEADER) + cntLen,
           "%s%s %jd\r\n%s", HEADER, "Content-Length:", src_stat.st_size,
           cntTyp);
  write(client_fd, header_payload, strlen(header_payload));
  write(client_fd, "\r\n", 2);

  char *buf = calloc(2 * MB_10, sizeof(char));
  size_t bytes_read = 0;
  while ((bytes_read = fread(buf, sizeof(char), 2 * MB_10, fp)) > 0) {
    write(client_fd, buf, bytes_read);
  }

  /*   write(client_fd, "\r\n", 2); */
  if (feof(fp))
    log_info("eof hit on %s", fn);
  fclose(fp);
  free(buf);
  free(fn);
  free(header_payload);
  free(cntTyp);
  return 0;
}

void strncatskip(char *dst, const char *src, size_t count, size_t offset) {
  int i, j;
  for (i = offset, j = strlen(dst); i < count; i++, j++) {
    dst[j] = src[i];
  }
  dst[j + 1] = '\0';
}
