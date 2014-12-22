#ifndef rvl_renderer_h
#define rvl_renderer_h

#include "rvl_scene.h"

void rvl_renderer_clean_up();

void rvl_renderer_draw(rvl_scene *scene, rvl_entity *me);

void rvl_renderer_init();

#endif

