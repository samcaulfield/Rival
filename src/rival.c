#include <stdio.h>
#include <stdlib.h>

#include "rvl_connection.h"
#include "rvl_entity.h"
#include "rvl_list.h"
#include "rvl_renderer.h"
#include "rvl_scene.h"
#include "rvl_skin.h"

#define ATTACK     'a'
#define END_TURN   'n'
#define INVENTORY  'i'
#define MOVE_DOWN  'j'
#define MOVE_LEFT  'h'
#define MOVE_RIGHT 'l'
#define MOVE_UP    'k'
#define QUIT       'q'

typedef enum {
        draw, ok, error, loss, quit, win
} result;

static bool is_end_condition(result r)
{
        return r == draw || r == error || r == loss || r == quit || r == win;
}

rvl_direction key_to_dir(char key)
{
        switch (key) {
        case MOVE_DOWN:
                return rvl_down;
        case MOVE_LEFT:
                return rvl_left;
        case MOVE_RIGHT:
                return rvl_right;
        case MOVE_UP:
                return rvl_up;
        }
        return rvl_none;
}

result do_combat(rvl_scene *scene, rvl_entity *me, rvl_entity *rival,
        rvl_entity *player, rvl_entity *waiting, rvl_entity *target,
        bool is_user)
{
        uint32_t p_old = player->health, t_old = target->health;
        rvl_scene_attack(player, target);
        rvl_scene_action(scene, player, waiting);
        
        /* Suppose combatants are actually the players. */
        if (me->health <= 0 && rival->health <= 0)
                return draw;
        else if (me->health <= 0)
                return loss;
        else if (rival->health <= 0)
                return win;
        else if (target->health <= 0) {
                /* Target died - loot it and remove it. */
                rvl_entity_add(player, target->inventory);
                if (is_user) {
                        char buffer[80];
                        sprintf(buffer, "Looted %d items.",
                                rvl_cdict_size(target->inventory));
                        rvl_renderer_add(scene, player, buffer);
                }

                uint32_t j = 0;
                for (j; j < rvl_scene_size(scene); j++)
                        if (target == rvl_scene_get(scene, j))
                                rvl_scene_remove(scene, j);
        }

        /* Print the results to the attacking player. */
        if (is_user) {
                char buffer[80];
                sprintf(buffer, "You did %d damage and took %d damage!",
                        t_old - target->health, p_old - player->health);
                        rvl_renderer_add(scene, player, buffer);
        } else
                rvl_renderer_draw(scene, me);

}

result handle_key(char key, rvl_entity *player, rvl_entity *waiting,
        bool is_user, rvl_scene *scene)
{
        /* Figure which player is "me" based on who sent the key. */
        rvl_entity *me = (is_user) ? player : waiting;
        rvl_entity *rival = (is_user) ? waiting : player;

        switch (key) {
        case ATTACK:
                if (is_user && !rvl_connection_send(ATTACK))
                        return error;
                rvl_list *nearby = rvl_scene_nearby(scene, player);
                if (rvl_list_size(nearby) == 0) {
                        if (is_user)
                                rvl_renderer_add(scene, me,
                                        "Nothing to attack.");
                } else if (rvl_list_size(nearby) == 1) {
                        result r = do_combat(scene, me, rival, player, waiting,
                                rvl_list_get(nearby, 0), is_user);
                } else {
                        uint32_t i;
                        rvl_entity *target = rvl_list_get(nearby, 0);
                        if (is_user) {
                                /* Colour in potential targets. */
                                target->colour = rvl_green;
                                i = 1;
                                for (i; i < rvl_list_size(nearby); i++)
                                        ((rvl_entity *) rvl_list_get(nearby, i))
                                                ->colour = rvl_red;
                                rvl_renderer_add(scene, player,
                                "Select a target. Press k to cycle targets, " \
                                "a to attack and c to exit.");
                        }
                        i = 0;
                        /* Target selection loop. */ 
                        char in = '\0';
                        do {
                                in = (is_user) ? getchar() :
                                        rvl_connection_recv();
                                if (is_user && !rvl_connection_send(in)) {
                                        return error;
                                        rvl_list_free(nearby, NULL);
                                }
                                if (in == 'k') {
                                        i++;
                                        if (i > rvl_list_size(nearby) - 1)
                                                i = 0;
                                        if (is_user)
                                                target->colour = rvl_red;
                                        target = rvl_list_get(nearby, i);
                                        if (is_user) {
                                                target->colour = rvl_green;
                                                rvl_renderer_draw(scene, me);
                                        }
                                }
                        } while (in != 'a' && in != 'c');
                        i = 0;
                        if (in == 'a') {
                                result r = do_combat(scene, me, rival, player,
                                        waiting, target, is_user);
                                if (is_end_condition(r))
                                        return r;
                        }

                        for (i; i < rvl_list_size(nearby); i++)
                                ((rvl_entity *) rvl_list_get(nearby, i))->colour
                                        = rvl_white;
                        rvl_renderer_draw(scene, me);
                }
                rvl_list_free(nearby, NULL);
                break;
        case END_TURN:
                player->moves = 0;
                waiting->moves = 5;
                if (is_user && !rvl_connection_send(END_TURN))
                        return error;
                rvl_renderer_draw(scene, me);
                break;
        case INVENTORY:
                if (is_user && !rvl_connection_send(INVENTORY))
                        return error;
                /* Open the inventory screen. */
                if (is_user) {
                        rvl_renderer_set_mode(rvl_renderer_mode_inv);
                        rvl_renderer_draw(scene, me);
                        char in;
                        while ((in = (is_user) ? getchar() :
                                rvl_connection_recv()) != 'i')
                                if (is_user && !rvl_connection_send(INVENTORY))
                                        return error;
                        rvl_renderer_set_mode(rvl_renderer_mode_game);
                        rvl_renderer_draw(scene, me);
                }
                break;
        case MOVE_DOWN:
        case MOVE_LEFT:
        case MOVE_RIGHT:
        case MOVE_UP:
                rvl_scene_move(scene, player, waiting, key_to_dir(key));
                if (is_user && !rvl_connection_send(key))
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
        rvl_entity *top_left = rvl_entity_new(0, 0, RVL_MOVES, 5, 5, 10, true,
                rvl_white, rvl_skin_white, rvl_player, NULL);
        rvl_entity *bottom_right = rvl_entity_new(scene->columns - 1,
                scene->rows - 1, 0, 5, 5, 10, true, rvl_white,
                rvl_skin_black, rvl_player, NULL);
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

        if (!rvl_renderer_init())
                return EXIT_FAILURE;
        rvl_renderer_draw(scene, me);

        result r;
        bool running = true;
        while (running) {
                while (running && me->moves) {
                        if (me->moves == RVL_MOVES)
                                rvl_renderer_add(scene, me, "It's your move!");                       
                        r = handle_key(getchar(), me, rival, true, scene);
                        if (is_end_condition(r))
                                running = false;
                }
                while (running && rival->moves) {
                        r = handle_key(rvl_connection_recv(), rival, me, false,
                                    scene);
                        if (is_end_condition(r))
                                running = false;
                }
        }

        rvl_renderer_clean_up();
        rvl_connection_close();

        switch (r) {
        case draw: printf("Draw!\n");
        case loss: printf("Loss!\n");
        case win: printf("Win!\n");
        }

        return EXIT_SUCCESS;
}

