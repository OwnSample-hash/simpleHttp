#include "linkList.h"
#include "parser.h"
#include <stdlib.h>

void insertAtEnd(splits_t *head, char *data) {
  splits_t newNode = createNode(data);
  if (*head == NULL) {
    *head = newNode;
    return;
  }
  splits_t temp = *head;
  while (temp->next != NULL) {
    temp = temp->next;
  }
  temp->next = newNode;
}

void insertAtBeginning(splits_t *head, char *data) {
  splits_t newNode = createNode(data);
  newNode->next = *head;
  *head = newNode;
}

splits_t createNode(char *data) {
  splits_t newNode = (splits_t)malloc(sizeof(splits_t));
  newNode->data = data;
  newNode->next = NULL;
  return newNode;
}

void deleteNode(splits_t *head, char *key) {
  splits_t temp = *head;
  splits_t prev = NULL;
  if (temp != NULL && temp->data == key) {
    *head = temp->next;
    free(temp);
    return;
  }
  while (temp != NULL && temp->data != key) {
    prev = temp;
    temp = temp->next;
  }
  if (temp == NULL) {
    return;
  }
  prev->next = temp->next;
  free(temp);
}

void freeList(splits_t head) {
  splits_t temp;
  while (head != NULL) {
    temp = head;
    head = head->next;
    free(temp);
  }
}
void displayList(splits_t head) {
  splits_t temp = head;
  while (temp != NULL) {
    printf("%s ", temp->data);
    temp = temp->next;
  }
  printf("\n");
}

void insertNode(splits_t *head, char *data) {
  splits_t newNode = (splits_t)malloc(sizeof(splits_t));
  newNode->data = data;
  newNode->next = NULL;

  if (*head == NULL) {
    *head = newNode;
    return;
  }

  splits_t temp = *head;
  while (temp->next != NULL) {
    temp = temp->next;
  }
  temp->next = newNode;
}
