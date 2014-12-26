#ifndef rvl_entity_h
#define rvl_entity_h

#include <stdbool.h>
#include <stdint.h>

#include "rvl_cdict.h"
#include "rvl_colour.h"
#include "rvl_item.h"
#include "rvl_skin.h"

typedef enum {
	rvl_player, rvl_stone, rvl_tree
} rvl_entity_type;

typedef struct {
	uint32_t x, y, moves, attack, defence, health;
	bool is_collidable;
	rvl_colour colour;
	rvl_skin skin;
	rvl_entity_type type;
	rvl_cdict *inventory;
} rvl_entity;

void rvl_entity_add(rvl_entity *e, rvl_cdict *items);

rvl_entity *rvl_entity_new(uint32_t x, uint32_t y, uint32_t moves,
	uint32_t attack, uint32_t defence, uint32_t health, bool is_collidable,
	rvl_colour colour, rvl_skin skin, rvl_entity_type type,
	rvl_cdict *inventory);

rvl_entity *rvl_entity_stone(uint32_t x, uint32_t y);

rvl_entity *rvl_entity_tree(uint32_t x, uint32_t y);

#endif

