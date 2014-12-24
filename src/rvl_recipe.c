#include "rvl_recipe.h"

static rvl_list *recipes = NULL;

rvl_list *rvl_recipe_get()
{
        if (!recipes) {
                rvl_cdict *wooden_club_i = rvl_cdict_new();
                if (!wooden_club_i)
                        return NULL;
                rvl_cdict_insert(wooden_club_i,
                        rvl_item_type_name(rvl_item_wood));
                rvl_cdict_insert(wooden_club_i,
                        rvl_item_type_name(rvl_item_wood));
                rvl_recipe *wooden_club_r = rvl_recipe_new(rvl_item_wooden_club,
                        1, wooden_club_i);
                if (!wooden_club_r)
                        return NULL;
                recipes = rvl_list_new();
                if (recipes)
                        rvl_list_insert(recipes, wooden_club_r);
                        
        }
        return recipes;
}

rvl_recipe *rvl_recipe_new(rvl_item_type result, uint32_t count,
        rvl_cdict *ingredients)
{
        rvl_recipe *new = (rvl_recipe *) malloc(sizeof(rvl_recipe));
        if (new) {
                new->result = result;
                new->count = count;
                new->ingredients = ingredients;
        }
        return new;
}

