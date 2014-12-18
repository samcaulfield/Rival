#include <stdlib.h>

#include "../include/rvlLinkedList.h"

static rvlError rvlLinkedNodeNewH(rvlLinkedNode **new)
{
  *new = (rvlLinkedNode *) malloc(sizeof(rvlLinkedNode));
  if (*new == NULL) {
    return rvlMemoryAllocationError;
  }
  (*new)->next = NULL;
  (*new)->previous = NULL;
  (*new)->data = NULL;
  return rvlNoError;
}

rvlError rvlLinkedListInsert(rvlLinkedList *list, void *data)
{
  rvlLinkedNode *newNode;
  rvlError error = rvlLinkedNodeNewH(&newNode);
  if (error != rvlNoError) {
    return error;
  }
  if (list->size == 0) {
    list->first = list->last = newNode;
    list->first->data = data;
  } else {
    rvlLinkedNode *temp = list->first;
    list->first = newNode;
    list->first->next = temp;
    temp->previous = list->first;
  }
  list->size++;
  return rvlNoError;
}

rvlError rvLinkedListNewH(rvlLinkedList **new)
{
  *new = (rvlLinkedList *) malloc(sizeof(rvlLinkedList));
  if (*new == NULL) {
    return rvlMemoryAllocationError;
  }
  (*new)->first = NULL;
  (*new)->last = NULL;
  (*new)->size = 0;
  return rvlNoError;
}

rvlError rvlLinkedListFreeH(rvlLinkedList *list, void (*freeFunction)(void *))
{
  rvlError error;
  uint32_t i = 0;
  rvlLinkedNode *node;
  while (i++ < list->size) {
    freeFunction(list->first->data);
    node = list->first;
    list->first = list->first->next;
    free(node);
  }
  free(list);
  return rvlNoError;
}

rvlError rvlLinkedListRemove(rvlLinkedList *list, uint32_t index,
  void (*freeFunction)(void *))
{
  if (index >= list->size) {
    return rvlNoError;
  }
  rvlLinkedNode *node = list->first;
  uint32_t i = 0;
  while (i++ < index) {
    node = node->next;
  }
  if (node == NULL) {
    return rvlNoError;
  }
  if (node->previous != NULL) {
    node->previous->next = node->next;
  } else { /* First node. */
    if (list->size == 1) {
      list->first = NULL;
    } else {
      list->first = node->next;
    }
  }
  if (node->next != NULL) {
    node->next->previous = node->previous;
  } else { /* Last node. */
    if (list->size == 1) {
      list->last = NULL;
    } else {
      list->last = node->previous;
    }
  }
  freeFunction(node->data);
  free(node);
  list->size--;
  return rvlNoError;
}

