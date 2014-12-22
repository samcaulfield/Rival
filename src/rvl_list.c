#include <stdlib.h>

#include "rvl_list.h"

static void free_node(rvl_node *node, void (*free_data) (void *))
{
        free_data(node->data);
        free(node);
}

static rvl_node *new_node(rvl_node *next, void *data)
{
        rvl_node* node = (rvl_node *) malloc(sizeof(rvl_node));
        if (node) {
                node->next = next;
                node->data = data;
        }
        return node;
}


void rvl_list_free(rvl_list *list, void (*free_data) (void *))
{
        rvl_node *temp;
        while (list->first) {
                temp = list->first;
                list->first = list->first->next;
                if (free_data)
                        free_node(temp, free_data);
        }
        free(list);
}

void *rvl_list_get(rvl_list *list, uint32_t i)
{
        rvl_node *node = list->first;
        while (i--)
                node = node->next;
        return node->data;
}

void rvl_list_insert(rvl_list *list, void *data)
{
        list->first = new_node(list->first, data);
        list->size++;
}

void rvl_list_remove(rvl_list *list, uint32_t i, void (*free_data) (void *))
{
        rvl_node *node = list->first, *prev = NULL;
        while (i--) {
                prev = node;
                node = node->next;
        }
        if (!prev)
                list->first = node->next;
        else
                prev->next = node->next;
        free_node(node, free_data);
        list->size--;
}

