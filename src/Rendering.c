#include <stdbool.h>
#include <stdio.h>

#include "../include/rvlScene.h"

void draw(rvlScene *scene)
{
  printf("[1;1H"); /* Set the cursor position to the top left. */
  int y = 0, x, i;
  bool printed;
  rvlPlayer *player;
  rvlEntity *entity;
  for (y; y < scene->length; y++) {
    x = 0;
    for (x; x < scene->width; x++) {
      i = 0;
      printed = false;
      for (i; i < scene->players->size; i++) {
        rvlLinkedListGet(scene->players, i, (void **) &player);
        if (player->entity->x == x && player->entity->y == y) {
          printf(player->entity->skin);
          printed = true;
        }
      }
      if (!printed) {
        i = 0;
        for (i; i < scene->entities->size; i++) {
          rvlLinkedListGet(scene->entities, i, (void **) &entity);
          if (entity->x == x && entity->y == y) {
            printf(entity->skin);
            printed = true;
          }
        }
        if (!printed)
          printf(".");
      }
    }
    printf("[E");
  }
}

static char *message0, *message1, *message2;

void addMessage(char *message)
{
  message2 = message1;
  message1 = message0;
  message0 = message;
}

void drawHelpText(rvlPlayer *me, bool myTurn, int movesLeft)
{
  printf("[41;1H"); /* Set the cursor position to after the map. */
  printf("[K"); /* Clear out existing text on this line. */
  printf("Controls: h,j,k,l: move, n: end turn, q: quit, a: attack");
  printf(" HP: %d, ATK: %d, DEF: %d", me->health, me->attack, me->defence);
  printf("[42;1H");
  printf("[K");
  printf((myTurn) ? "It's your turn! Moves left: %d."
    : "It's your opponent's turn!", movesLeft);
  printf("[43;1H");
  printf("[K");
  printf(message0);
  printf("[44;1H");
  printf("[K");
  printf(message1);
  printf("[45;1H");
  printf("[K");
  printf(message2);
}

