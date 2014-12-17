#include <stdlib.h>

#include "../include/Scene.h"

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
  return abs(a.x - b.x) + abs(a.y - b.y);
}

