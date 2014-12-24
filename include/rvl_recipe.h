#ifndef rvl_recipe_h
#define rvl_recipe_h

#include "rvl_cdict.h"
#include "rvl_item.h"
#include "rvl_list.h"

typedef struct {
        rvl_item_type result;
        uint32_t count;
        rvl_cdict *ingredients;
} rvl_recipe;

rvl_list *rvl_recipe_get();

rvl_recipe *rvl_recipe_new(rvl_item_type result, uint32_t count,
        rvl_cdict *ingredients);

#endif

