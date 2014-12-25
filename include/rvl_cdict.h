#ifndef rvl_cdict_h
#define rvl_cdict_h

#include <stdint.h>
#include <stdlib.h>

typedef struct rvl_cnode rvl_cnode;

struct rvl_cnode {
        struct rvl_cnode *next;
        char *key;
        uint32_t count;
};

typedef struct {
        rvl_cnode *first;
        uint32_t size;
} rvl_cdict;

void rvl_cdict_dec(rvl_cdict *dict, char *key);

void rvl_cdict_free(rvl_cdict *dict);

uint32_t rvl_cdict_get(rvl_cdict *dict, char *key);

char *rvl_cdict_i(rvl_cdict *dict, uint32_t i);

void rvl_cdict_insert(rvl_cdict *dict, char *key);

void rvl_cdict_less(rvl_cdict *a, rvl_cdict *b);

#define rvl_cdict_new() ((rvl_cdict *) calloc(1, sizeof(rvl_cdict)))

void rvl_cdict_remove(rvl_cdict *dict, char *key);

#define rvl_cdict_size(dict) (dict->size)

#endif

