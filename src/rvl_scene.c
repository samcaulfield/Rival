#include <stdlib.h>
#include <time.h>

#include "rvl_entity.h"
#include "rvl_scene.h"

void rvl_scene_action(rvl_scene *s, rvl_entity *a, rvl_entity *b)
{
        a->moves--;
        if (!a->moves)
                b->moves = RVL_MOVES;
}

void rvl_scene_attack(rvl_entity *a, rvl_entity *b)
{
        int damage = b->attack - a->defence;
        a->health -= ((damage > 0) ? damage : 0);
        damage = a->attack - b->defence;
        b->health -= ((damage > 0) ? damage : 0);
}

bool rvl_scene_can_move(rvl_scene *scene, int x, int y)
{
        /* Bounds check. */
        if (!(x >= 0 && x < scene->columns && y >= 0 && y < scene->rows))
                return false;

        /* Collision check. */
        rvl_entity *e;
        uint32_t i = 0;
        for (i; i < rvl_scene_size(scene); i++) {
                e = rvl_scene_get(scene, i);
                if (e->is_collidable && e->x == x && e->y == y)
                        return false;
        }
        return true;
}

uint32_t rvl_scene_distance(rvl_entity *a, rvl_entity *b)
{
        return abs(a->x - b->x) + abs(a->y - b->y);
}

bool rvl_scene_generate(rvl_scene *scene)
{
        srand(time(NULL));
        bool occupied = false;
        rvl_entity *e, *new;
        rvl_list *inv;
        int num_entities = scene->rows * scene->columns, x, y, i, index;
        while (num_entities-- > 0) {
                x = rand() % scene->columns;
                y = rand() % scene->rows;
                i = 0;
                for (i; i < rvl_scene_size(scene) && !occupied; i++) {
                        e = rvl_scene_get(scene, i);
                        if (e->x ==x && e->y == y)
                                occupied = true;
                }
                if (!occupied) {
                        int r = rand() % 30;
                        if (r < 6)
                                rvl_scene_add(scene, rvl_entity_stone(x, y));
                        else
                                rvl_scene_add(scene, rvl_entity_tree(x, y));
                }
        }
        return true;
}

void rvl_scene_move(rvl_scene *scene, rvl_entity *player, rvl_entity *waiting,
        rvl_direction dir)
{
        switch (dir) {
        case rvl_down:
                if (rvl_scene_can_move(scene, player->x, player->y + 1)) {
                        player->y++;
                        rvl_scene_action(scene, player, waiting);
                }
                break;
        case rvl_left:
                if (rvl_scene_can_move(scene, player->x - 1, player->y)) {
                        player->x--;
                        rvl_scene_action(scene, player, waiting);
                }
                break;
        case rvl_right:
                if (rvl_scene_can_move(scene, player->x + 1, player->y)) {
                        player->x++;
                        rvl_scene_action(scene, player, waiting);
                }
                break;
        case rvl_up:
                if (rvl_scene_can_move(scene, player->x, player->y - 1)) {
                        player->y--;
                        rvl_scene_action(scene, player, waiting);
                }
                break;
        }
}

rvl_list *rvl_scene_nearby(rvl_scene *scene, rvl_entity *entity)
{
        rvl_list *nearby = rvl_list_new();
        if (nearby) {
                rvl_entity *near;
                uint32_t i = 0;
                for (i; i < rvl_scene_size(scene); i++) {
                        near = rvl_scene_get(scene, i);
                        if (rvl_scene_distance(entity, near) == 1)
                                rvl_list_insert(nearby, near);
                }
        }
        return nearby;
}

rvl_scene *rvl_scene_new(int rows, int columns, rvl_list *list)
{
        rvl_scene *scene = (rvl_scene *) malloc(sizeof(rvl_scene));
        if (scene) {
                scene->rows = rows;
                scene->columns = columns;
                scene->entities = list;
        }
        return scene;
}

