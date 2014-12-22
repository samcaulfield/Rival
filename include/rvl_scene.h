#ifndef rvl_scene_h
#define rvl_scene_h

#include <stdbool.h>

#include "rvl_list.h"

#define RVL_MOVES 5

typedef enum {
        rvl_down, rvl_left, rvl_none, rvl_right, rvl_up
} rvl_direction;

typedef struct {
        uint32_t rows, columns;
        rvl_list *entities;
} rvl_scene;

#define rvl_scene_add(scene, entity) (rvl_list_insert(scene->entities, entity))

void rvl_scene_attack(rvl_entity *a, rvl_entity *b);

#define rvl_scene_get(scene, i) (rvl_list_get(scene->entities, i))

bool rvl_scene_can_move(rvl_scene *scene, int x, int y);

uint32_t rvl_scene_distance(rvl_entity *a, rvl_entity *b);

bool rvl_scene_generate(rvl_scene *scene);

void rvl_scene_move(rvl_scene *scene, rvl_entity *player, rvl_entity *waiting,
        rvl_direction dir);

rvl_list *rvl_scene_nearby(rvl_scene *scene, rvl_entity *entity);

rvl_scene *rvl_scene_new(int rows, int columns, rvl_list *entities);

#define rvl_scene_remove(scene, i) (rvl_list_remove(scene->entities, i, free))

#define rvl_scene_size(scene) (rvl_list_size(scene->entities))

#endif

