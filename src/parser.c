#include "parser.h"
#include "itoa.h"
#include "link_list.h"
#include "log/log.h"
#include "lua/setup.h"
#include "lua/virtual_path.h"
#include "mime_guess.h"
#include "write_response.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
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
                                "<p>Not Found</p>\r\n"
                                "</body>\r\n"
                                "</html>\r\n";
  write(client_fd, payload, sizeof(payload));
}

/*splits_t splitOn_c(char *str, const char *delimiter) {*/
/*  splits_t head = NULL;*/
/*  char *token = strtok(str, delimiter);*/
/**/
/*  while (token != NULL) {*/
/*    insertNode(&head, token);*/
/*    token = strtok(NULL, delimiter);*/
/*  }*/
/**/
/*  return head;*/
/*}*/

node_t *splitOn_c(char *str, const char *delimiter) {
  node_t *head = NULL;
  char *token = strtok(str, delimiter);

  while (token != NULL) {
    insertNode(&head, token, strlen(token) + 1);
    token = strtok(NULL, delimiter);
  }

  return head;
}

parseGet_t parseReq(char *request, size_t srequest, int client_fd,
                    const char *root, const keep_alive_t *keep_alive) {
  log_info("Proccessing %ld bytes request", srequest);
  log_debug("errno: %d", errno);
  node_t *lines = splitOn_c(request, "\n");
  char *cpyData = lines->data;
  node_t *wordsGet = splitOn_c(cpyData, " ");
  // wordsGet[0] == GET
  // wordsGet[1] == $PATH
  // wordsGet[2] == HTTP/1.1
  if (strncmp(wordsGet->next->next->data, "HTTP/1.1", 8) != 0) {
    log_error("Request is not HTTP/1.1 got %s ", wordsGet->next->next->data);
    return WRONG_PROTOCOL;
  }
  log_debug("errno: %d", errno);
  if (wordsGet->next != NULL) {
    if (strcmp(wordsGet->next->data, "/") == 0)
      wordsGet->next->data = "/index.html";
  } else
    return 1;
  log_debug("errno: %d", errno);
  char *payload;
  int payload_len = 0;
  payload_len += strlen(wordsGet->data);
  payload_len += strlen(wordsGet->next->data);
  payload = calloc(payload_len + 2, sizeof(char));
  log_debug("errno: %d", errno);
  snprintf(payload, payload_len + 2, "%s %s", (char *)wordsGet->data,
           (char *)wordsGet->next->data);
  log_debug("errno: %d", errno);
  int ret = parseGet(payload, strlen(payload), client_fd, root, keep_alive);
  log_debug("errno: %d", errno);
  if (ret == NOT_FOUND) {
    log_trace("praseGet(...) != OK_GET is true, ret: %d", ret);
    log_info("Treating path (%s) as a virtual path", wordsGet->next->data);
    ret = virtual_path_resolv(wordsGet->next->data, client_fd, wordsGet->data,
                              keep_alive);
    log_debug("errno: %d", errno);
  }
  log_debug("errno: %d", errno);
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
  log_debug("errno: %d", errno);
  freeList(lines);
  freeList(wordsGet);
  free(payload);
  log_trace("Returing %d", ret);
  return ret;
}

parseGet_t parseGet(char *payload, size_t spayload, int client_fd,
                    const char *root, const keep_alive_t *keep_alive) {
  log_debug("errno: %d", errno);
  char *fn = calloc(spayload + 3, sizeof(char));
  if (fn == NULL) {
    log_fatal("CALLOC FAILED");
    perror("calloc");
    return CALLOC;
  }
  /*strncpy(fn, root, strlen(root));*/
  log_debug("errno: %d", errno);
  snprintf(fn, spayload + 2, "%s", root);
  strncatskip(fn, payload, spayload, 4);
  log_info("Opening file: '%s' on root: '%s'", fn, root);
  FILE *fp = fopen(fn, "rb");
  if (fp == NULL) {
    return NOT_FOUND;
  }
  log_debug("errno: %d", errno);

  struct stat src_stat = {0};
  if (stat(fn, &src_stat)) {
    perror("stat");
    free(fn);
    return NO_STAT;
  }

  log_debug("errno: %d", errno);
  response_t *resp = new_response(client_fd, lookup_status(200), keep_alive);
  if (resp == NULL) {
    log_error("Failed to create response");
    fclose(fp);
    free(fn);
    return ERR;
  }

  char *cntTyp = ContentType(M_FILE, fn);
  log_debug("errno: %d", errno);
  add_header(resp, "Content-Type", cntTyp);
  log_debug("errno: %d", errno);
  free(cntTyp);
  add_header(resp, "Content-Length", TO_BASE(src_stat.st_size, 10));

  log_debug("errno: %d", errno);
  char *buf = calloc(src_stat.st_size, sizeof(char));
  if (buf == NULL) {
    perror("praseGet,calloc");
    log_fatal("prasGet,calloc");
    fclose(fp);
    free(fn);
    return CALLOC;
  }
  log_debug("errno: %d", errno);
  off_t bytes_read = fread(buf, sizeof(char), src_stat.st_size, fp);
  log_debug("errno: %d", errno);
  /*while ((bytes_read = ) > 0) {*/
  /*}*/
  if (bytes_read != src_stat.st_size) {
    log_error("Failed to read file");
    free(buf);
    fclose(fp);
    free(fn);
    return ERR;
  }
  set_payload(resp, buf, bytes_read);
  log_debug("errno: %d", errno);
  log_trace("Wrote %ju byte to client %d", write_response(resp), client_fd);
  free_response(resp);

  src_stat = (struct stat){0};
  /*if (feof(fp))*/
  /*  log_trace("eof hit on %s", fn);*/
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
