/**
 * @file
 * @brief A one-way linked list
 */

#ifndef __LINK_LIST_H__
#define __LINK_LIST_H__

#include <stddef.h>

/**
 * @struct node
 * @brief Represenst a node in the linked list
 *
 * @var node::size
 * @brief The size of the data that this node holds
 *
 * @var node::data
 * @brief A pointer to the data
 *
 * @var node::next
 * @brief The next node
 *
 */
typedef struct node {
  size_t size;
  void *data;
  struct node *next;
} node_t;
/**<@copydoc node*/

/**
 * @brief Creates a new node and copies the data to it
 *
 * @param data Pointer to the data to store
 * @param size Size of the data
 * @return The node that holds that data
 */
node_t *create_node(void *data, size_t size);

/**
 * @brief Inserts a node to a list
 *
 * @param head The head of the list, if null will create a new list
 * @param data Data to insert
 * @param size Size of the data to insert
 */
void insert_node(node_t **head, void *data, size_t size);

/*[[gnu::unused]] void deleteNode(node_t **head, void *key);*/

/**
 * @brief Free a list
 *
 * @param head List to free
 */
void free_list(node_t *head);

/**
 * @brief Free a list with a custom free function
 *
 * @param head List to free
 * @param freeFunc Function to free the data in each node
 */
void free_list_custom(node_t *head, void (*freeFunc)(void *));
/**
 * @brief Prints the list as strings unused and marked as deprecated
 *
 * @param head List to print
 */
[[deprecated("Not deprecated, just unused")]]
void display_list(node_t *head);

#endif // __LINK_LIST_H__
// Vim: set expandtab tabstop=2 shiftwidth=2:
