#include <stdlib.h>

#include "rvl_entity.h"

void rvl_entity_add(rvl_entity *e, rvl_list *items)
{
        uint32_t i = 0;
        for (i; i < rvl_list_size(items); i++)
                rvl_list_insert(e->inventory, rvl_list_get(items, i));
}

rvl_entity *rvl_entity_new(uint32_t x, uint32_t y, uint32_t moves,
        uint32_t attack, uint32_t defence, uint32_t health, bool is_collidable,
        rvl_colour colour, rvl_skin skin, rvl_entity_type type,
        rvl_list *inventory)
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
                entity->inventory = (inventory) ? inventory : rvl_list_new();
        }
        return entity;
}

rvl_entity *rvl_entity_stone(uint32_t x, uint32_t y)
{
        rvl_list *inv = rvl_list_new();
        if (!inv)
                return NULL;
        rvl_list_insert(inv, rvl_item_new(rvl_item_stone));
        return rvl_entity_new(x, y, 0, 0, 4, 10, true, rvl_white,
                rvl_skin_stone, rvl_stone, inv);
}

rvl_entity *rvl_entity_tree(uint32_t x, uint32_t y)
{
        rvl_list *inv = rvl_list_new();
        if (!inv)
                return NULL;
        rvl_list_insert(inv, rvl_item_new(rvl_item_wood));
        return rvl_entity_new(x, y, 0, 0, 3, 10, true, rvl_white,
                rvl_skin_tree, rvl_tree, inv);
}

