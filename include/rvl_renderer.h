#ifndef rvl_renderer_h
#define rvl_renderer_h

#include "rvl_scene.h"

void rvl_renderer_add(rvl_scene *scene, rvl_entity *me, char *msg);

void rvl_renderer_clean_up();

void rvl_renderer_draw(rvl_scene *scene, rvl_entity *me);

bool rvl_renderer_init();

void rvl_renderer_inv(rvl_scene *scene, rvl_entity *me);

#endif

