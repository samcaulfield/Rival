#ifndef rvlLinkedList_h
#define rvlLinkedList_h

#include <stdint.h>

#include "rvlError.h"

typedef struct rvlLinkedNode rvlLinkedNode;

struct rvlLinkedNode {
  struct rvlLinkedNode *next, *previous;
  void *data;
};

typedef struct {
  struct rvlLinkedNode *first, *last;
  uint32_t size;
} rvlLinkedList;

rvlError rvlLinkedListGet(rvlLinkedList *list, uint32_t index, void **data);

rvlError rvlLinkedListInsert(rvlLinkedList *list, void *data);

rvlError rvlLinkedListNewH(rvlLinkedList **new);

rvlError rvlLinkedListFree(rvlLinkedList *list, void (*freeFunction)(void *));

rvlError rvlLinkedListRemove(rvlLinkedList *list, uint32_t inxex,
  void (*freeFunction)(void *));

#endif

