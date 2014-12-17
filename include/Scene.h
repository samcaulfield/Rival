#ifndef SCENE_H
#define SCENE_H

#define WIDTH 80
#define LENGTH 40

struct Player {
  int attack, defence, health, x, y;
  char *skin;
};

void attack(struct Player *a, struct Player *b);

int distance(struct Player a, struct Player b);

#endif

