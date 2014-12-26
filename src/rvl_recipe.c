#include "rvl_recipe.h"

static rvl_list *recipes = NULL;

rvl_list *rvl_recipe_craftable(rvl_cdict *items)
{
	rvl_recipe_get();
	rvl_list *craftable_r = rvl_list_new();
	if (!craftable_r)
		return NULL;
	rvl_recipe *recipe;
	rvl_cdict *ingr;
	char *key;
	uint32_t i = 0, j, val;
next:   for (i; i < rvl_list_size(recipes); i++) {
		recipe = (rvl_recipe *) rvl_list_get(recipes, i);
		ingr = recipe->ingredients;
		j = 0;
		for (j; j < rvl_cdict_size(ingr); j++) {
			key = rvl_cdict_i(ingr, j);
			val = rvl_cdict_get(ingr, key);
			if (rvl_cdict_get(items, key) < val) {
				i++;
				goto next;
			}
		}
		rvl_list_insert(craftable_r, recipe);
	}
	return craftable_r;
}

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

