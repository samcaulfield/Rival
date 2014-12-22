#include <stdio.h>
#include <stdlib.h>

#include "rvl_connection.h"
#include "rvl_entity.h"
#include "rvl_list.h"
#include "rvl_renderer.h"
#include "rvl_scene.h"

#define END_TURN   'n'
#define MOVE_DOWN  'j'
#define MOVE_LEFT  'h'
#define MOVE_RIGHT 'l'
#define MOVE_UP    'k'
#define QUIT       'q'

typedef enum {
        ok, error, quit
} result;

result handle_key(char key, rvl_entity *player, rvl_entity *waiting,
        bool is_user, rvl_scene *scene)
{
        /* Figure which player is "me" based on who sent the key. */
        rvl_entity *me = (is_user) ? player : waiting;

        switch (key) {
        case END_TURN:
                player->moves = 0;
                waiting->moves = 5;
                if (is_user && !rvl_connection_send(END_TURN))
                        return error;
                rvl_renderer_draw(scene, me);
                break;
        case MOVE_DOWN:
                if (rvl_scene_can_move(scene, player->x, player->y + 1))
                        rvl_scene_move(scene, player, waiting, rvl_down);
                if (is_user && !rvl_connection_send(MOVE_DOWN))
                        return error;
                rvl_renderer_draw(scene, me);
                break;
        case MOVE_LEFT:
                if (rvl_scene_can_move(scene, player->x - 1, player->y))
                        rvl_scene_move(scene, player, waiting, rvl_left);
                if (is_user && !rvl_connection_send(MOVE_LEFT))
                        return error;
                rvl_renderer_draw(scene, me);
                break;
        case MOVE_RIGHT:
                if (rvl_scene_can_move(scene, player->x + 1, player->y))
                        rvl_scene_move(scene, player, waiting, rvl_right);
                if (is_user && !rvl_connection_send(MOVE_RIGHT))
                        return error;
                rvl_renderer_draw(scene, me);
                break;
        case MOVE_UP:
                if (rvl_scene_can_move(scene, player->x, player->y - 1))
                        rvl_scene_move(scene, player, waiting, rvl_up);
                if (is_user && !rvl_connection_send(MOVE_UP))
                        return error;
                rvl_renderer_draw(scene, me);
                break;
        case QUIT:
                if (is_user && !rvl_connection_send(QUIT))
                        return error;
                return quit;
                break;
        }
        return ok;
}

int main(int argc, char **argv)
{
        /* Set the scene. */
        rvl_list *entities = rvl_list_new();
        if (!entities)
                return EXIT_FAILURE;
        rvl_scene *scene = rvl_scene_new(40, 80, entities);
        if (!scene)
                return EXIT_FAILURE;

        /* The two players. top_left goes first. */
        rvl_entity *top_left = rvl_entity_new(0, 0, 5, 5, 5, 10, true, "\u263b",
                rvl_player);
        rvl_entity *bottom_right = rvl_entity_new(scene->columns - 1,
                scene->rows - 1, 0, 5, 5, 10, true, "\u263a", rvl_player);
        if (!top_left || !bottom_right)
                return EXIT_FAILURE;

        rvl_scene_add(scene, top_left);
        rvl_scene_add(scene, bottom_right);

        bool client = argc > 1;

        /* Set up the network connection based on command-line arguments. */
        rvl_connection_new((client) ? argv[1] : NULL);

        /* Generate the terrain after the connection is made, so that the seed
         * for terrain generation (time) is the same for both players. */
        if (!rvl_scene_generate(scene))
                return EXIT_FAILURE;


        /* The player who waited for the connection goes first. */
        rvl_entity *me, *rival;
        me = (client) ? bottom_right : top_left;
        rival = (client) ? top_left : bottom_right;

        rvl_renderer_init();
        rvl_renderer_draw(scene, me);

        result r;
        bool running = true;
        while (running) {
                while (running && me->moves) {
                        r = handle_key(getchar(), me, rival, true, scene);
                        if (r == error || r == quit)
                                running = false;
                }
                while (running && rival->moves) {
                        r = handle_key(rvl_connection_recv(), rival, me, false,
                                    scene);
                        if (r == error || r == quit)
                                running = false;
                }
        }

        rvl_renderer_clean_up();
        rvl_connection_close();

        return EXIT_SUCCESS;
}

