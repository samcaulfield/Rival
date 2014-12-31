#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "rvl_entity.h"
#include "rvl_recipe.h"
#include "rvl_renderer.h"
#include "rvl_skin.h"

static rvl_renderer_mode mode;

#define cursor_down_next()   (printf("[E"))
#define clear_screen()       (printf("[2J"))
#define clear_line()	 (printf("[K"))
#define set_cursor_pos(x, y) (printf("[%d;%dH", x, y))
#define hide_cursor()	(printf("[?25l"))
#define show_cursor()	(printf("[?25h"))

#define BUF_SIZE 3
static rvl_list *msg_buf;

static void add(char *msg)
{
	char *new = malloc(strlen(msg) * sizeof(char));
	strcpy(new, msg);
	rvl_list_insert(msg_buf, new);
	if (rvl_list_size(msg_buf) > BUF_SIZE)
		rvl_list_remove(msg_buf, BUF_SIZE, free);
}

static void game(rvl_scene *scene, rvl_entity *me)
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

static rvl_list *craftable = NULL;
static uint32_t inv_select = 0;

static void inv(rvl_scene *scene, rvl_entity *me)
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
	craftable = rvl_recipe_craftable(me->inventory);
	if (inv_select >= rvl_list_size(craftable))
		inv_select = 0;
	i = 0;
	for (i; i < rvl_list_size(craftable); i++) {
		cursor_down_next();
		if (i == inv_select)
			printf("%s", rvl_colour_begin(rvl_green));
		printf("%s", rvl_item_type_name(
			((rvl_item_type) (((rvl_recipe *) 
				rvl_list_get(craftable, i))->result))));
		if (i == inv_select)
			printf("%s", rvl_colour_end());
	}
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
	switch (mode) {
	case rvl_renderer_mode_game:
		game(scene, me);
		break;
	case rvl_renderer_mode_inv:
		inv(scene, me);
		break;
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
	mode = rvl_renderer_mode_game;
	return true;
}

void *rvl_renderer_key(char key)
{
	switch (mode) {
	case rvl_renderer_mode_inv:
		switch (key) {
		case 'c':
			if (inv_select < rvl_list_size(craftable))
				return rvl_list_get(craftable, inv_select);
			break;
		}
		break;
	}
	return NULL;
}

void rvl_renderer_set_mode(rvl_renderer_mode new_mode)
{
      mode = new_mode;
}

