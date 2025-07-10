#include "parser.h"
#include "link_list.h"
#include "log/log.h"
#include "lua/virtual_path.h"
#include "spec.h"
#include "write_response.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/**
 * @struct request
 * @brief The request struct opaque struct
 *
 * @var request::cfd
 * Client file descriptor
 *
 * @var request::method
 * request method
 *
 * @var request::path
 * Uri path
 *
 * @var request::path_length
 * Path lenght
 *
 * @var request::headers
 * Linked list to the headers
 *
 * @var request::body
 * Body of the request if any
 *
 * @var request::body_length
 * Lenght of the body request if any
 *
 * @var request::query_params
 * Query params
 */
struct request {
  int cfd;
  http_method_t method;
  const char *path;
  size_t path_length;
  node_t *headers;
  const char *body;
  size_t body_length;

  node_t *query_params; // List of query parameters
};

request_t *init_request(int cfd) {
  request_t *req = calloc(sizeof(request_t), 1);
  if (req == NULL) {
    log_error("Failed to allocate memory for request");
    return NULL; // Memory allocation failed
  }
  req->cfd = cfd;
  req->method = HTTP_METHOD_UNKNOWN;
  req->path = NULL;
  req->headers = NULL;
  req->body = NULL;
  return req;
}

void free_request(request_t *req) {
  free_list(req->headers);
  free((void *)req->path);
  free((void *)req->body);
  free(req);
}

extern node_t *splitOn_c(char *str, const char *delimiter) {
  node_t *head = NULL;
  char *token = strtok(str, delimiter);

  while (token != NULL) {
    insert_node(&head, token, strlen(token) + 1);
    token = strtok(NULL, delimiter);
  }

  return head;
}

const char *method_to_string(http_method_t method) {
  switch (method) {
  case HTTP_METHOD_GET:
    return "GET";
  case HTTP_METHOD_POST:
    return "POST";
  case HTTP_METHOD_PUT:
    return "PUT";
  case HTTP_METHOD_DELETE:
    return "DELETE";
  case HTTP_METHOD_PATCH:
    return "PATCH";
  case HTTP_METHOD_OPTIONS:
    return "OPTIONS";
  case HTTP_METHOD_HEAD:
    return "HEAD";
  default:
    return "UNKNOWN";
  }
}

parse_status_t parse_request(request_t *req) {
  if (req == NULL) {
    log_error("Request is NULL");
    return PARSE_ERR_INVALID_REQUEST;
  }
  FILE *fp = fdopen(req->cfd, "r");
  if (fp == NULL) {
    log_error("Failed to open file descriptor for reading");
    return PARSE_ERR; // Failed to open file descriptor
  }

  char buffer[1024];
  if (fgets(buffer, sizeof(buffer), fp) == NULL) {
    log_error("Failed to read request line");
    fclose(fp);
    return PARSE_ERR; // Failed to read request line
  }
  node_t *lines = splitOn_c(buffer, " ");
  if (lines == NULL || lines->next == NULL) {
    log_error("Invalid request format");
    fclose(fp);
    return PARSE_ERR_INVALID_REQUEST; // Invalid request format
  }
  if (strcmp(lines->data, "GET") == 0) {
    req->method = HTTP_METHOD_GET;
  } else if (strcmp(lines->data, "POST") == 0) {
    req->method = HTTP_METHOD_POST;
  } else if (strcmp(lines->data, "PUT") == 0) {
    req->method = HTTP_METHOD_PUT;
  } else if (strcmp(lines->data, "DELETE") == 0) {
    req->method = HTTP_METHOD_DELETE;
  } else if (strcmp(lines->data, "PATCH") == 0) {
    req->method = HTTP_METHOD_PATCH;
  } else if (strcmp(lines->data, "OPTIONS") == 0) {
    req->method = HTTP_METHOD_OPTIONS;
  } else if (strcmp(lines->data, "HEAD") == 0) {
    req->method = HTTP_METHOD_HEAD;
  }
  req->path = strdup(lines->next->data);
  if (req->path == NULL) {
    log_error("Failed to allocate memory for URI");
    free_list(lines);
    fclose(fp);
    return PARSE_ERR_NO_MEMORY;
  }
  req->path_length = strlen(req->path);

  // Process headers

  while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    if (strcmp(buffer, "\r\n") == 0 || strcmp(buffer, "\n") == 0) {
      break; // End of headers
    }
    char *header_line = strdup(buffer);
    if (header_line == NULL) {
      log_error("Failed to allocate memory for header line");
      free_list(lines);
      fclose(fp);
      return PARSE_ERR_NO_MEMORY;
    }
    node_t *header_node = splitOn_c(header_line, ": ");
    if (header_node != NULL && header_node->next != NULL) {
      pair_ss_t *header_pair = calloc(sizeof(pair_ss_t), 1);
      if (header_pair == NULL) {
        log_error("Failed to allocate memory for header pair");
        free(header_line);
        free_list(lines);
        fclose(fp);
        return PARSE_ERR_NO_MEMORY;
      }
      header_pair->v1 = header_node->data;
      header_pair->v2 = header_node->next->data;
      insert_node(&req->headers, header_pair, sizeof(pair_ss_t));
      free(header_pair);
    }
    free_list(header_node);
    free(header_line);
  }

  // node_t *temp = req->headers;
  // while (temp != NULL) {
  //   log_debug("%s: %s", ((pair_ss_t *)temp->data)->key,
  //             ((pair_ss_t *)temp->data)->value);
  //   temp = temp->next;
  // }

  free_list(lines);
  int fd = dup(req->cfd);
  if (fd == -1) {
    log_error("Failed to duplicate file descriptor");
    fclose(fp);
    return PARSE_ERR; // Failed to duplicate file descriptor
  }
  fclose(fp);
  req->cfd = fd;
  return PARSE_OK; // Successfully parsed request
}

parse_status_t process_request(request_t *req, const char *root,
                               const keep_alive_t *keep_alive) {
  if (req == NULL || root == NULL || keep_alive == NULL) {
    log_error("Invalid parameters for processing request");
    return PARSE_ERR_INVALID_REQUEST;
  }
  if (req->method == HTTP_METHOD_UNKNOWN) {
    log_error("Unknown HTTP method");
    return PARSE_ERR_INVALID_METHOD;
  }
  log_info("Processing request: Method: %s, URI: %s",
           method_to_string(req->method), req->path);

  vlookup_status ret = virtual_path_resolv(
      req->path, req->cfd, method_to_string(req->method), keep_alive);

  response_t *res = new_response(req->cfd, lookup_status(200), keep_alive);

  switch (ret) {
  case OK_LOOKUP:
    log_info("Virtual path resolved successfully");
    return PARSE_OK; // Successfully processed request
  case HANDLED:
    log_info("Virtual path handled by Lua script");
    return PARSE_OK; // Handled by Lua script
  case WRONG_VERB:
    log_error("Wrong HTTP verb for virtual path");
    sr(res, (spec_response_t){.status = 405, .data = "Method Not Allowed"});
    free_response(res);
    return PARSE_ERR_INVALID_METHOD; // Wrong HTTP method
  case NIL:
    log_error("Virtual path resolution returned NIL");
    sr(res, (spec_response_t){.status = 404, .data = "Not Found"});
    free_response(res);
    return PARSE_ERR_INVALID_URI; // Invalid URI
  case STATIC:
    log_info("Static file served for virtual path");
    break;
  }

  const char *tmp = NULL;
  const char *index = "/index.html";
  if (strcmp(req->path, "/") <= 0) {
    tmp = req->path;
    req->path = index;
  }
  char *file = calloc(strlen(root) + strlen(req->path) + 2, sizeof(char));
  if (file == NULL) {
    log_error("Failed to allocate memory for file name");
    sr(res, (spec_response_t){.status = 500, .data = "Internal Server Error"});
    free_response(res);
    free(file);
    return PARSE_ERR_NO_MEMORY; // Memory allocation failed
  }
  snprintf(file, strlen(root) + strlen(req->path) + 2, "%s%s", root, req->path);

  log_info("Serving static file: %s", file);

  if (res == NULL) {
    log_error("Failed to create response");
    free(file);
    sr(res, (spec_response_t){.status = 500, .data = "Internal Server Error"});
    free_response(res);
    return PARSE_ERR_NO_MEMORY; // Failed to create response
  }

  struct stat st;

  if (stat(file, &st) == -1) {
    log_error("File not found: %s", file);
    free(file);
    sr(res, (spec_response_t){.status = 404, .data = "Not Found"});
    free_response(res);
    return PARSE_ERR_FILE_NOT_FOUND; // File not found
  }

  if (!S_ISREG(st.st_mode)) {
    log_error("Not a regular file: %s", file);
    free(file);
    return PARSE_ERR_INVALID_REQUEST; // Not a regular file
  }

  if (st.st_size == 0) {
    log_warn("Empty file: %s", file);
    free(file);
    sr(res, (spec_response_t){.status = 204, .data = "No Content"});
    free_response(res);
    return PARSE_OK; // Empty file, nothing to serve
  }

  add_file_to_payload(res, file, "rb");
  free(file);

  size_t bytes_written = write_response(res);
  if (bytes_written == (size_t)-1) {
    log_error("Failed to write response to client");
    free_response(res);
    return PARSE_ERR_WRITE_FAILED; // Failed to write response
  }
  log_info("Bytes written to client %d: %zu", req->cfd, bytes_written);
  if (tmp != NULL)
    req->path = tmp; // Restore original path if it was modified
  free_response(res);
  return PARSE_OK; // Successfully processed request
}
// Vim: set expandtab tabstop=2 shiftwidth=2:
