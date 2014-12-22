#ifndef rvl_entity_h
#define rvl_entity_h

#include <stdbool.h>
#include <stdint.h>

typedef enum {
        rvl_player, rvl_tree
} rvl_entity_type;

typedef struct {
        uint32_t x, y, moves, attack, defence, health;
        bool is_collidable;
        char *skin;
        rvl_entity_type type;
} rvl_entity;

rvl_entity *rvl_entity_new(uint32_t x, uint32_t y, uint32_t moves,
        uint32_t attack, uint32_t defence, uint32_t health, bool is_collidable,
        char *skin, rvl_entity_type type);

#endif

