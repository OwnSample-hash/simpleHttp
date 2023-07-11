#ifndef __LINKLIST_HTTP__
#define __LINKLIST_HTTP__

#include "parser.h"

typedef struct __node {
  void *data;
  struct __node *next;
} *node_t;

typedef struct __list {
  int size;
  node_t *node;
} list_t;

#ifdef __NEW_LINKLIST__

list_t createNode(void *data);
void insertAtBeginning(list_t *Node, void *data);
void deleteNode(list_t *Node, void *key);
void freeList(list_t head);
void displayList(list_t head);
void insertNode(list_t *head, void *data);

#else

splits_t createNode(char *data);
void insertAtBeginning(splits_t *Node, char *data);
void deleteNode(splits_t *Node, char *key);
void freeList(splits_t head);
void displayList(splits_t head);
void insertNode(splits_t *head, char *data);

#endif

#endif /* __LINKLIST_HTTP__ */
