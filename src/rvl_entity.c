#include <stdlib.h>

#include "rvl_entity.h"

rvl_entity *rvl_entity_new(uint32_t x, uint32_t y, uint32_t moves,
        uint32_t attack, uint32_t defence, uint32_t health, bool is_collidable,
        rvl_skin skin, rvl_entity_type type)
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
                entity->skin = skin;
                entity->type = type;
        }
        return entity;
}

