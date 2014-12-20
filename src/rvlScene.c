#include <stdlib.h>
#include <time.h>

#include "../include/rvlLinkedList.h"
#include "../include/rvlScene.h"

void generateTerrain(rvlScene *scene)
{
  srand(time(NULL)); /* Seed the random generation nondeterministically. */
  /* The number of entities generated is a function of the size of the map. */
  int numEntities = scene->length * scene->width / 50, index, x, y, i;
  bool occupied;
  rvlEntity *entity, *scanEntity;
  rvlPlayer *scanPlayer;
  while (numEntities-- > 0) {
    index = rand() % (scene->length * scene->width); /* Get a map position. */
    x = index % scene->width;
    y = index / scene->width;
    i = 0;
    occupied = false;
    for (i; i < scene->entities->size; i++) { /* Prevent double ups. */
      rvlLinkedListGet(scene->entities, i, (void **) &scanEntity);
      if (scanEntity->x == x && scanEntity->y == y)
        occupied = true;
    }
    i = 0;
    for (i; i < scene->players->size; i++) {
      rvlLinkedListGet(scene->players, i, (void **) &scanPlayer);
      if (scanPlayer->entity->x == x && scanPlayer->entity->y == y)
        occupied = true;
    }
    if (!occupied) {
      rvlEntityNewH(&entity, x, y, true, "\u2660");
      rvlLinkedListInsert(scene->entities, entity);
    }
  }
}

bool canMoveTo(rvlScene *scene, int x, int y)
{
  if (scene == NULL)
    return false;
  rvlPlayer *player;
  uint32_t i = 0;
  for (i; i < scene->players->size; i++) {
    rvlLinkedListGet(scene->players, i, (void **) &player);
    if (player->entity->x == x && player->entity->y == y)
      return false;
  }
  rvlEntity *entity;
  i = 0;
  for (i; i < scene->entities->size; i++) {
    rvlLinkedListGet(scene->entities, i, (void **) &entity);
    if (entity->x == x && entity->y == y)
      return false;
  }
  return true;
}

rvlError rvlSceneAddPlayer(rvlScene *scene, rvlPlayer *player)
{
  rvlLinkedListInsert(scene->players, player);
  return rvlNoError;
}

rvlError rvlEntityNewH(rvlEntity **new, int x, int y, bool isCollidable,
  char *skin)
{
  *new = (rvlEntity *) malloc(sizeof(rvlEntity));
  if (*new == NULL)
    return rvlMemoryAllocationError;
  (*new)->x = x;
  (*new)->y = y;
  (*new)->isCollidable = isCollidable;
  (*new)->skin = skin;
  return rvlNoError;
}

rvlError rvlPlayerNewH(rvlPlayer **new, rvlEntity *entity, int attack,
  int defence, int health)
{
  *new = (rvlPlayer *) malloc(sizeof(rvlPlayer));
  if (*new == NULL)
    return rvlMemoryAllocationError;
  (*new)->entity = entity;
  (*new)->attack = attack;
  (*new)->defence = defence;
  (*new)->health = health;
  return rvlNoError;
}

rvlError rvlSceneNewH(rvlScene **new, int width, int length)
{
  *new = (rvlScene *) malloc(sizeof(rvlScene));
  if (*new == NULL)
    return rvlMemoryAllocationError;
  (*new)->width = width;
  (*new)->length = length;
  rvlError error;
  if ((error = rvlLinkedListNewH(&(*new)->players)) != rvlNoError) {
    free(*new);
    return error;
  }
  if ((error = rvlLinkedListNewH(&(*new)->entities)) != rvlNoError) {
    free(*new);
    free((*new)->players);
    return error;
  }
  return rvlNoError;
}

rvlError rvlPlayerFree(rvlPlayer *player)
{
  free(player->entity);
  free(player);
}

void attack(rvlPlayer *a, rvlPlayer *b)
{
  int aStrength = a->attack - b->defence;
  aStrength = (aStrength < 0) ? 0 : aStrength;
  b->health -= aStrength;
  int bStrength = b->attack - a->defence;
  bStrength = (bStrength < 0) ? 0 : bStrength;
  a->health -= bStrength;
}

int distance(rvlEntity *a, rvlEntity *b)
{
  return abs(a->x - b->x) + abs(a->y - b->y);
}

