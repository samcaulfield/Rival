#include <stdlib.h>
#include <time.h>

#include "rvl_entity.h"
#include "rvl_scene.h"

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

bool rvl_scene_generate(rvl_scene *scene)
{
        srand(time(NULL));
        bool occupied = false;
        rvl_entity *e, *new;
        int num_entities = scene->rows * scene->columns / 50, x, y, i, index;
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
                        new = rvl_entity_new(x, y, 0, 0, 3, 10, true,
                                rvl_skin_tree, rvl_tree);
                        if (!new)
                                return false;
                        rvl_scene_add(scene, new);
                }
        }
        return true;
}

void rvl_scene_move(rvl_scene *scene, rvl_entity *player, rvl_entity *waiting,
        rvl_direction dir)
{
        switch (dir) {
        case rvl_down:
                player->y++;
                break;
        case rvl_left:
                player->x--;
                break;
        case rvl_right:
                player->x++;
                break;
        case rvl_up:
                player->y--;
                break;
        }
        player->moves--;
        if (!player->moves)
                waiting->moves = RVL_MOVES;
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

