#include <stdlib.h>
#include <time.h>

#include "../include/rvlLinkedList.h"
#include "../include/rvlScene.h"

rvlEntity *rvlSceneGetEntity(rvlScene *s, uint32_t index)
{
  if (s == NULL || index >= rvlSceneGetNumEntities(s))
    return NULL;
  rvlEntity *e;
  rvlLinkedListGet(s->entities, index, (void **) &e);
  return e;
}

uint32_t rvlSceneGetNumEntities(rvlScene *s)
{
  if (s == NULL)
    return 0;
  return s->entities->size;
}

uint32_t getNearby(rvlScene *scene, rvlEntity *entity)
{
  if (scene == NULL || entity == NULL)
    return 0;
  uint32_t i = 0, nearby = 0;
  rvlEntity *scanEntity;
  for (i; i < scene->entities->size; i++) {
    rvlLinkedListGet(scene->entities, i, (void **) scanEntity);
    if (distance(entity, scanEntity) == 1)
      nearby++;
  }
  return nearby;
}

void addEntity(rvlScene *scene, rvlEntity *entity)
{
  if (scene == NULL)
    return;
  rvlLinkedListInsert(scene->entities, entity);
}

void generateTerrain(rvlScene *scene)
{
  srand(time(NULL)); /* Seed the random generation nondeterministically. */
  /* The number of entities generated is a function of the size of the map. */
  int numEntities = scene->length * scene->width / 50, index, x, y, i;
  bool occupied;
  rvlEntity *entity, *scanEntity;
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
    if (!occupied) {
      rvlEntityNewH(&entity, x, y, true, "\u2660", 0, 1, 10, Tree);
      rvlLinkedListInsert(scene->entities, entity);
    }
  }
}

bool canMoveTo(rvlScene *scene, int x, int y)
{
  if (scene == NULL)
    return false;
  uint32_t i = 0;
  rvlEntity *entity;
  i = 0;
  for (i; i < scene->entities->size; i++) {
    rvlLinkedListGet(scene->entities, i, (void **) &entity);
    if (entity->x == x && entity->y == y)
      return false;
  }
  return true;
}

rvlError rvlEntityNewH(rvlEntity **new, int x, int y, bool isCollidable,
  char *skin, int attack, int defence, int health, EntityType type)
{
  *new = (rvlEntity *) malloc(sizeof(rvlEntity));
  if (*new == NULL)
    return rvlMemoryAllocationError;
  (*new)->x = x;
  (*new)->y = y;
  (*new)->isCollidable = isCollidable;
  (*new)->skin = skin;
  (*new)->attack = attack;
  (*new)->defence = defence;
  (*new)->health = health;
  (*new)->type = type;
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
  if ((error = rvlLinkedListNewH(&(*new)->entities)) != rvlNoError) {
    free(*new);
    return error;
  }
  return rvlNoError;
}

void attack(rvlEntity *a, rvlEntity *b)
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

