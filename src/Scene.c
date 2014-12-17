#include <stdlib.h>

#include "../include/Scene.h"

struct Entity NewEntity(int x, int y, bool isCollidable, char *skin)
{
  struct Entity entity;
  entity.x = x;
  entity.y = y;
  entity.isCollidable = isCollidable;
  entity.skin = skin;
  return entity;
}

struct Player NewPlayer(struct Entity entity, int attack, int defence,
  int health)
{
  struct Player player;
  player.entity = entity;
  player.attack = attack;
  player.defence = defence;
  player.health = health;
  return player;
}

void attack(struct Player *a, struct Player *b)
{
  int aStrength = a->attack - b->defence;
  aStrength = (aStrength < 0) ? 0 : aStrength;
  b->health -= aStrength;
  int bStrength = b->attack - a->defence;
  bStrength = (bStrength < 0) ? 0 : bStrength;
  a->health -= bStrength;
}

int distance(struct Player a, struct Player b)
{
  return abs(a.entity.x - b.entity.x) + abs(a.entity.y - b.entity.y);
}

