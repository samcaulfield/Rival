#ifndef SCENE_H
#define SCENE_H

#include <stdbool.h>

#define WIDTH 80
#define LENGTH 40

struct Entity {
  int x, y;
  bool isCollidable;
  char *skin;
};

struct Player {
  struct Entity entity;
  int attack, defence, health;
};

struct Entity NewEntity(int x, int y, bool isCollidable, char *skin);

struct Player NewPlayer(struct Entity entity, int attack, int defence,
  int health);

void attack(struct Player *a, struct Player *b);

int distance(struct Player a, struct Player b);

#endif

