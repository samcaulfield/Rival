#ifndef rvl_list_h
#define rvl_list_h

#include <stdint.h>

typedef struct rvl_node rvl_node;

struct rvl_node {
	struct rvl_node *next;
	void *data;
};

typedef struct {
	struct rvl_node *first;
	uint32_t size;
} rvl_list;

#define rvl_list_new() ((rvl_list *) calloc(1, sizeof(rvl_list)))

void rvl_list_free(rvl_list *list, void (*free_data) (void *));

void *rvl_list_get(rvl_list *list, uint32_t i);

void rvl_list_insert(rvl_list *list, void *data);

void rvl_list_remove(rvl_list *list, uint32_t i, void (*free_data) (void *));

#define rvl_list_size(list) (list->size)

#endif

