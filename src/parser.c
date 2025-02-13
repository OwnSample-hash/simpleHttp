#include "parser.h"
#include "itoa.h"
#include "linkList.h"
#include "log/log.h"
#include "lua/setup.h"
#include "lua/virtual_path.h"
#include "mime_guess.h"
#include "write_response.h"
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
  if (strncmp(wordsGet->next->next->data, "HTTP/1.1", 8) != 0) {
    log_error("Request is not HTTP/1.1 got %s ", wordsGet->next->next->data);
    return WRONG_PROTOCOL;
  }
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
  if (ret == NOT_FOUND) {
    log_trace("praseGet(...) != OK_GET is true, ret: %d", ret);
    log_info("Treating path (%s) as a virtual path", wordsGet->next->data);
    ret = virtual_path_resolv(wordsGet->next->data, client_fd, wordsGet->data,
                              keep_alive);
  }
  switch (ret) {
  case NO_STAT:
    log_error("Failed to stat file");
    erep(client_fd);
    break;
  case WRONG_PROTOCOL:
    log_error("Wrong protocol");
    erep(client_fd);
    break;
  case ERR:
    log_error("Error");
    break;
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
    free(fn);
    return NO_STAT;
  }

  response_t *resp = newRespones(client_fd, lookupStatus(200), keep_alive);
  if (resp == NULL) {
    log_error("Failed to create response");
    fclose(fp);
    free(fn);
    return ERR;
  }

  char *cntTyp = ContentType(M_FILE, fn);
  addHeader2Response(resp, "Content-Type", cntTyp);
  free(cntTyp);
  addHeader2Response(resp, "Content-Length", TO_BASE(src_stat.st_size, 10));

  char *buf = calloc(src_stat.st_size, sizeof(char));
  if (buf == NULL) {
    perror("praseGet,calloc");
    log_fatal("prasGet,calloc");
    fclose(fp);
    free(fn);
    return CALLOC;
  }
  off_t bytes_read = fread(buf, sizeof(char), src_stat.st_size, fp);
  /*while ((bytes_read = ) > 0) {*/
  /*}*/
  if (bytes_read != src_stat.st_size) {
    log_error("Failed to read file");
    free(buf);
    fclose(fp);
    free(fn);
    return ERR;
  }
  setPayload(resp, buf, bytes_read);
  log_trace("Wrote %ju byte to client %d", writeResponse(resp), client_fd);

  src_stat = (struct stat){0};
  if (feof(fp))
    log_trace("eof hit on %s", fn);
  fclose(fp);
  free(buf);
  free(fn);
  /*free(header_payload);*/
  /*free(header);*/
  return OK_GET;
}

void strncatskip(char *dst, const char *src, size_t count, size_t offset) {
  size_t i, j;
  for (i = offset, j = strlen(dst); i < count; i++, j++)
    dst[j] = src[i];
  dst[j] = '\0';
}
