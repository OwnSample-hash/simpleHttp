#include "parser.h"
#include "itoa.h"
#include "linkList.h"
#include <assert.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void erep(int client_fd) {
  static const char payload[] = "HTTP/1.1 404 Not Found\r\n"
                                "Connection: close\r\n"
                                "Content-Length: 67\r\n"
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
  // lines[0] == GET / HTTP/1.1
  // TODO:
  //  resolve / => /index.html done
  //  remove HTTP/1.1 from the end done
  //  parse and log more info from the req
  char *cpyData = lines->data;
  splits_t wordsGet = splitOn_c(cpyData, (const char *)" ");
  // wordsGet[0] == GET
  // wordsGet[1] == $PATH
  // wordsGet[2] == HTTP/1.1
  if (wordsGet->next != NULL) {
    if (strcmp(wordsGet->next->data, "/") == 0)
      wordsGet->next->data = "/index.html";
    // else
    //   return 1;
  } else
    return 1;
  char *payload;
  int payload_len = 0;
  payload_len += strlen(wordsGet->data);
  payload_len += strlen(wordsGet->next->data);
  payload = calloc(payload_len + 2, sizeof(char));
  snprintf(payload, payload_len + 2, "%s %s", wordsGet->data,
           wordsGet->next->data);
  /*  printf("Calling parseGet with %s payload\n", payload); */
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
  // fn[strlen (fn) - 1] = '\0';
  printf("Opening file: '%s'\n", fn);
  FILE *fp = fopen(fn, "rb");
  // raise(SIGTRAP);
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
  char *shit = TO_BASE(src_stat.st_size, 10);
  int len_strelen = strlen(shit);
  char *header_payload = calloc(20 + len_strelen + strlen(HEADER),
                                sizeof(char)); // "Content-Length: "
  snprintf(header_payload, 20 + len_strelen + strlen(HEADER), "%s%s %jd\r\n",
           HEADER, "Content-Length:", src_stat.st_size);
  write(client_fd, header_payload, strlen(header_payload));
  write(client_fd, "\r\n", 2);

  char *buf = calloc(BUFSIZ * 2, sizeof(char));
  size_t bytes_read;
  while ((bytes_read = fread(buf, sizeof(char), sizeof(buf), fp)) > 0) {
    write(client_fd, buf, bytes_read);
  }

  write(client_fd, "\r\n", 2);
  if (feof(fp))
    printf("eof hit on %s\n", fn);
  fclose(fp);
  free(buf);
  free(fn);
  free(header_payload);
  return 0;
}

void strncatskip(char *dst, const char *src, size_t count, size_t offset) {
  int i, j;
  for (i = offset, j = strlen(dst); i < count; i++, j++) {
    dst[j] = src[i];
  }
  dst[j + 1] = '\0';
}
