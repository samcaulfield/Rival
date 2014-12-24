#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "rvl_entity.h"
#include "rvl_recipe.h"
#include "rvl_renderer.h"
#include "rvl_skin.h"

#define cursor_down_next()   (printf("[E"))
#define clear_screen()       (printf("[2J"))
#define clear_line()         (printf("[K"))
#define set_cursor_pos(x, y) (printf("[%d;%dH", x, y))
#define hide_cursor()        (printf("[?25l"))
#define show_cursor()        (printf("[?25h"))

#define BUF_SIZE 3
static rvl_list *msg_buf;

static void add(char *msg)
{
        char *new = (char *) malloc(strlen(msg) * sizeof(char));
        strcpy(new, msg);
        rvl_list_insert(msg_buf, new);
        if (rvl_list_size(msg_buf) > BUF_SIZE)
                rvl_list_remove(msg_buf, BUF_SIZE, free);
}

void rvl_renderer_add(rvl_scene *scene, rvl_entity *me, char *msg)
{
        add(msg);
        rvl_renderer_draw(scene, me);
}

void rvl_renderer_clean_up()
{
        show_cursor();
        clear_screen();
        system("/bin/stty cooked");
        struct termios settings;
        tcgetattr(STDIN_FILENO, &settings);
        settings.c_lflag |= ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &settings);
        set_cursor_pos(1, 1);
        rvl_list_free(msg_buf, free);
}

void rvl_renderer_draw(rvl_scene *scene, rvl_entity *me)
{
        set_cursor_pos(1, 1);
        bool printed = false;
        rvl_entity *e;
        uint32_t x, y = 0, i;
        for (y; y < scene->rows; y++) {
                x = 0;
                for (x; x < scene->columns; x++) {
                        i = 0;
                        printed = false;
                        for (i; i < rvl_scene_size(scene); i++) {
                                e = rvl_scene_get(scene, i);
                                if (x == e->x && y == e->y) {
                                        printf("%s%s%s",
                                                rvl_colour_begin(e->colour),
                                                e->skin,
                                                rvl_colour_end());
                                        printed = true;
                                }
                        }
                        if (!printed)
                                printf("%s", rvl_skin_bare);
                }
                cursor_down_next();
        }

        /* Draw status information. */
        clear_line();
        printf("Controls: h,j,k,l: move, n: end turn, q: quit, a: attack");
        printf("  ");
        printf("ATK: %d, DEF: %d, HP: %d", me->attack, me->defence, me->health);
        cursor_down_next();
        if (!me->moves)
                printf("%s", rvl_colour_begin(rvl_red));
        printf("Moves left: %d.", me->moves);
        if (!me->moves)
                printf("%s", rvl_colour_end());
        uint32_t m = 0;
        while (m < BUF_SIZE) {
                cursor_down_next();
                clear_line();
                printf("%s", (char *) rvl_list_get(msg_buf, m++));
        }
}

bool rvl_renderer_init()
{
        setbuf(stdout, NULL);
        hide_cursor();
        clear_screen();
        system("/bin/stty raw");
        struct termios settings;
        tcgetattr(STDIN_FILENO, &settings);
        settings.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &settings);
        if ((msg_buf = rvl_list_new()) == NULL)
                return false;
        uint32_t i = 0;
        for (i; i < BUF_SIZE; i++)
                add("");
        return true;
}

void rvl_renderer_inv(rvl_scene *scene, rvl_entity *me)
{
        clear_screen();
        set_cursor_pos(1, 1);
        printf("Inventory:");
        uint32_t i = 0;
        char *key;
        for (i; i < rvl_cdict_size(me->inventory); i++) {
                cursor_down_next();
                key = rvl_cdict_i(me->inventory, i);
                printf("%s %d", key, rvl_cdict_get(me->inventory, key));
        }
        set_cursor_pos(21, 1);
        printf("Crafting:");
        rvl_list *recipes = rvl_recipe_get();
        rvl_recipe *recipe;
        uint32_t j;
        i = 0;
next:
        for (i; i < rvl_list_size(recipes); i++) {
                recipe = (rvl_recipe *) rvl_list_get(recipes, i);
                rvl_cdict *ing = recipe->ingredients;
                j = 0;
                for (j; j < rvl_cdict_size(ing); j++) {
                        char *key = rvl_cdict_i(ing, j);
                        uint32_t val = rvl_cdict_get(ing, key);
                        uint32_t my_val = rvl_cdict_get(me->inventory, key);
                        if (my_val < val) {
                                i++;
                                goto next;
                        }
                                
                }
                cursor_down_next();
                printf("%s", rvl_item_type_name(recipe->result));
        }
}

