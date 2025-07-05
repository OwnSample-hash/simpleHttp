#include "link_list.h"
#include "log/log.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

node_t *createNode(void *data, size_t size) {
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

void insertNode(node_t **head, void *data, size_t size) {
  node_t *newNode = createNode(data, size);
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

void freeList(node_t *head) {
  perror("current errno");
  errno = 0;
  perror("current errno");
  node_t *temp;
  int i = 0;
  while (head != NULL) {
    temp = head;
    head = head->next;
    log_trace("Freeing node %d", i++);
    free(temp->data);
    free(temp);
  }
}

void displayList(node_t *head) {
  node_t *temp = head;
  while (temp != NULL) {
    log_debug("%s", (char *)temp->data);
    temp = temp->next;
  }
}
