#ifndef rvl_renderer_h
#define rvl_renderer_h

#include "rvl_scene.h"

typedef enum {
	rvl_renderer_mode_inv, rvl_renderer_mode_game
} rvl_renderer_mode;

void rvl_renderer_add(rvl_scene *scene, rvl_entity *me, char *msg);

void rvl_renderer_clean_up();

void rvl_renderer_draw(rvl_scene *scene, rvl_entity *me);

bool rvl_renderer_init();

void *rvl_renderer_key(char key);

void rvl_renderer_set_mode(rvl_renderer_mode new_mode);

#endif

