#include <string.h>

#include "rvl_cdict.h"

static rvl_cnode *new_node(rvl_cnode *next, char *key)
{
        rvl_cnode *node = (rvl_cnode *) malloc(sizeof(rvl_cnode));
        if (node) {
                node->count = 1;
                node->next = next;
                node->key = key;
        }
        return node;
}

void rvl_cdict_free(rvl_cdict *dict)
{
        rvl_cnode *temp;
        while (dict->first) {
                temp = dict->first;
                dict->first = dict->first->next;
                free(temp->key);
                free(temp);
        }
        free(dict);
}

uint32_t rvl_cdict_get(rvl_cdict *dict, char *key)
{
        rvl_cnode *node = dict->first;
        while (strcmp(node->key, key))
                node = node->next;
        return node->count;
}

char *rvl_cdict_i(rvl_cdict *dict, uint32_t i)
{
        rvl_cnode *node = dict->first;
        while (i--)
                node = node->next;
        return node->key;
}

void rvl_cdict_insert(rvl_cdict *dict, char *key)
{
        rvl_cnode *node = dict->first;
        while (node && strcmp(key, node->key) != 0)
                node = node->next;
        if (node)
                node->count++;
        else {
                char *buf = (char *) calloc(strlen(key), sizeof(char));
                strcpy(buf, key);
                dict->first = new_node(dict->first, buf);
                dict->size++;
        }
}

void rvl_cdict_remove(rvl_cdict *dict, char *key)
{
        rvl_cnode *node = dict->first, *prev = NULL;
        while (!strcmp(key, node->key)) {
                prev = node;
                node = node->next;
        }
        if (!prev)
                dict->first = node->next;
        else
                prev->next = node->next;
        free(node->key);
        free(node);
        dict->size--;
}

