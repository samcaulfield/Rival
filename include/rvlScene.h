#ifndef rvlScene_h
#define rvlScene_h

#include <stdbool.h>

#include "rvlLinkedList.h"

typedef enum {
  Player, Tree
} EntityType;

typedef struct {
  int x, y;
  bool isCollidable;
  char *skin;
  int attack, defence, health;
  EntityType type;
} rvlEntity;

typedef struct {
  int width, length;
  rvlLinkedList *entities;
} rvlScene;

uint32_t getNearby(rvlScene *scene, rvlEntity *entity);

void generateTerrain(rvlScene *scene);

bool canMoveTo(rvlScene *scene, int x, int y);

void addEntity(rvlScene *scene, rvlEntity *entity);

rvlError rvlEntityNewH(rvlEntity **new, int x, int y, bool isCollidable,
  char *skin, int attack, int defence, int health, EntityType type);

rvlError rvlSceneNewH(rvlScene **new, int width, int length);

void attack(rvlEntity *a, rvlEntity *b);

int distance(rvlEntity *a, rvlEntity *b);

#endif

