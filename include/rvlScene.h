#ifndef rvlScene_h
#define rvlScene_h

#include <stdbool.h>

#include "rvlLinkedList.h"

typedef struct {
  int x, y;
  bool isCollidable;
  char *skin;
} rvlEntity;

typedef struct {
  rvlEntity *entity;
  int attack, defence, health;
} rvlPlayer;

typedef struct {
  int width, length;
  rvlLinkedList *entities;
  rvlLinkedList *players;
} rvlScene;

void generateTerrain(rvlScene *scene);

bool canMoveTo(rvlScene *scene, int x, int y);

rvlEntity rvlEntityNew(int x, int y, bool isCollidable, char *skin);
rvlPlayer rvlPlayerNew(rvlEntity entity, int attack, int defence, int health);

rvlError rvlEntityNewH(rvlEntity **new, int x, int y, bool isCollidable,
  char *skin);

rvlError rvlPlayerNewH(rvlPlayer **new, rvlEntity *entity, int attack,
  int defence, int health);

rvlError rvlSceneNewH(rvlScene **new, int width, int length);

rvlError rvlPlayerFree(rvlPlayer *player);

rvlError rvlSceneAddPlayer(rvlScene *scene, rvlPlayer *player);

void attack(rvlPlayer *a, rvlPlayer *b);

int distance(rvlEntity *a, rvlEntity *b);

#endif

