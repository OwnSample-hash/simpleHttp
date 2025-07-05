#ifndef __LINK_LIST_H__
#define __LINK_LIST_H__

#include <stddef.h>

typedef struct node {
  size_t size;
  void *data;
  struct node *next;
} node_t;

node_t *createNode(void *data, size_t size);

void insertNode(node_t **head, void *data, size_t size);

/*[[gnu::unused]] void deleteNode(node_t **head, void *key);*/

void freeList(node_t *head);

void displayList(node_t *head);

#endif // __LINK_LIST_H__
