#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "rvl_entity.h"
#include "rvl_renderer.h"

#define cursor_down_next()   (printf("[E"))
#define clear_screen()       (printf("[2J"))
#define clear_line()         (printf("[K"))
#define set_cursor_pos(x, y) (printf("[%d;%dH", x, y))
#define hide_cursor()        (printf("[?25l"))
#define show_cursor()        (printf("[?25h"))

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
                                        printf("%s", e->skin);
                                        printed = true;
                                }
                        }
                        if (!printed)
                                printf(".");
                }
                cursor_down_next();
        }

        /* Draw status information. */
        clear_line();
        printf("Controls: h,j,k,l: move, n: end turn, q: quit, a: attack");
        printf("  ");
        printf("ATK: %d, DEF: %d, HP: %d", me->attack, me->defence, me->health);
}

void rvl_renderer_init()
{
        setbuf(stdout, NULL);
        hide_cursor();
        clear_screen();
        system("/bin/stty raw");
        struct termios settings;
        tcgetattr(STDIN_FILENO, &settings);
        settings.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &settings);
}

