#ifndef __LINKLIST_HTTP__
#define __LINKLIST_HTTP__

#include "parser.h"

splits_t createNode (char *data);
void insertAtBeginning (splits_t *Node, char *data);
void deleteNode (splits_t *Node, char *key);
void freeList (splits_t head);
void displayList (splits_t head);
void insertNode (splits_t *head, char *data);
#endif /* __LINKLIST_HTTP__ */
