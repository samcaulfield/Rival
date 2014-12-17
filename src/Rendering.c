#include <stdbool.h>
#include <stdio.h>

#include "../include/Scene.h"

void draw(int width, int length, struct Player me, struct Player rival)
{
  printf("[1;1H");
  int y = 0, x, index;
  for (y; y < length; y++) {
    x = 0;
    for (x; x < width; x++) {
      if (x == me.entity.x && y == me.entity.y) {
        printf(me.entity.skin);
      } else if (x == rival.entity.x && y == rival.entity.y) {
        printf(rival.entity.skin);
      } else {
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

void drawHelpText(struct Player me, bool myTurn, int movesLeft)
{
  printf("[41;1H");
  printf("[K");
  printf("Controls: h,j,k,l: navigate, n: end turn, q: quit");
  printf("         HP: %d, ATK: %d, DEF: %d", me.health, me.attack, me.defence);
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

