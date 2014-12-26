#include <stdlib.h>

#include "rvl_entity.h"

void rvl_entity_add(rvl_entity *e, rvl_cdict *items)
{
	uint32_t i = 0, j, k;
	char *key;
	for (i; i < rvl_cdict_size(items); i++) {
		key = rvl_cdict_i(items, i);
		j = rvl_cdict_get(items, key);
		k = 0;
		for (k; k < j; k++)
			rvl_cdict_insert(e->inventory, key);
	}
}

rvl_entity *rvl_entity_new(uint32_t x, uint32_t y, uint32_t moves,
	uint32_t attack, uint32_t defence, uint32_t health, bool is_collidable,
	rvl_colour colour, rvl_skin skin, rvl_entity_type type,
	rvl_cdict *inventory)
{
	rvl_entity *entity = (rvl_entity *) malloc(sizeof(rvl_entity));
	if (entity) {
		entity->x = x;
		entity->y = y;
		entity->moves = moves;
		entity->attack = attack;
		entity->defence = defence;
		entity->health = health;
		entity->is_collidable = is_collidable;
		entity->colour = colour;
		entity->skin = skin;
		entity->type = type;
		entity->inventory = (inventory) ? inventory : rvl_cdict_new();
	}
	return entity;
}

rvl_entity *rvl_entity_stone(uint32_t x, uint32_t y)
{
	rvl_cdict *inv = rvl_cdict_new();
	if (!inv)
		return NULL;
	rvl_cdict_insert(inv, rvl_item_type_name(rvl_item_stone));
	return rvl_entity_new(x, y, 0, 0, 4, 10, true, rvl_white,
		rvl_skin_stone, rvl_stone, inv);
}

rvl_entity *rvl_entity_tree(uint32_t x, uint32_t y)
{
	rvl_cdict *inv = rvl_cdict_new();
	if (!inv)
		return NULL;
	rvl_cdict_insert(inv, rvl_item_type_name(rvl_item_wood));
	return rvl_entity_new(x, y, 0, 0, 3, 10, true, rvl_white,
		rvl_skin_tree, rvl_tree, inv);
}

