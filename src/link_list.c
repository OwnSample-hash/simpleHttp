#include "link_list.h"
#include "log/log.h"
#include <stdlib.h>
#include <string.h>

node_t *create_node(void *data, size_t size) {
  if (data == NULL || size == 0) {
    log_error("Invalid data or size");
    return NULL;
  }
  node_t *newNode = calloc(1, sizeof(node_t));
  if (newNode == NULL) {
    return NULL;
  }
  newNode->data = calloc(size, 1);
  if (newNode->data == NULL) {
    free(newNode);
    return NULL;
  }
  memcpy(newNode->data, data, size);
  newNode->next = NULL;
  newNode->size = size;
  return newNode;
}

void insert_node(node_t **head, void *data, size_t size) {
  node_t *newNode = create_node(data, size);
  if (*head == NULL) {
    *head = newNode;
    return;
  }
  node_t *temp = *head;
  while (temp->next != NULL) {
    temp = temp->next;
  }
  temp->next = newNode;
}

void free_list(node_t *head) {
  node_t *temp;
  while (head != NULL) {
    temp = head;
    head = head->next;
    free(temp->data);
    free(temp);
  }
}

void free_list_custom(node_t *head, void (*freeFunc)(void *)) {
  node_t *temp;
  while (head != NULL) {
    temp = head;
    head = head->next;
    if (temp->data && freeFunc) {
      freeFunc(temp->data);
    }
    free(temp);
  }
}

void display_list(node_t *head) {
  node_t *temp = head;
  while (temp != NULL) {
    log_debug("%s", (char *)temp->data);
    temp = temp->next;
  }
}
// Vim: set expandtab tabstop=2 shiftwidth=2:
