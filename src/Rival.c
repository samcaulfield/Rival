#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "../include/rvlError.h"
#include "../include/Network.h"
#include "../include/rvlScene.h"
#include "../include/Rendering.h"

struct {
  char moveUp, moveRight, moveDown, moveLeft, endTurn, attack, quit;
} keyBindings;

rvlScene *scene;
rvlPlayer *me, *rival;
int movesLeft;

typedef enum {
  EndTurn, ExitOk, ExitError, Nothing
} action;

action keyPress(char key)
{
  if (key == keyBindings.attack) {
    if (movesLeft) {
      if (distance(me->entity, rival->entity) <= 1) {
        int meHp = me->health, rivalHp = rival->health;
        attack(me, rival);
	int meLost = me->health - meHp,
          rivalLost = rivalHp - rival->health;
	if (sendMessage(keyBindings.attack) == -1)
	  return ExitError;
	draw(scene);
        movesLeft--;
        addMessage("Attack!");
      } else {
        addMessage("Nothing to attack.");
      }
    }
  } else if (key == keyBindings.endTurn) {
    if (sendMessage(keyBindings.endTurn) == -1)
      return ExitError;
    return EndTurn;
  } else if (key == keyBindings.moveUp) {
    if (movesLeft) {
      if (me->entity->y > 0) {
        me->entity->y--;
        draw(scene);
        if (sendMessage(keyBindings.moveUp) == -1)
          return ExitError;
        movesLeft--;
      }
    }
  } else if (key == keyBindings.moveLeft) {
    if (movesLeft) {
      if (me->entity->x > 0) {
        me->entity->x--;
        draw(scene);
        if (sendMessage(keyBindings.moveLeft) == -1)
          return ExitError;
        movesLeft--;
      }
    }
  } else if (key == keyBindings.moveDown) {
    if (movesLeft) {
      if (me->entity->y < scene->length - 1) {
        me->entity->y++;
        draw(scene);
        if (sendMessage(keyBindings.moveDown) == -1)
          return ExitError;
        movesLeft--;
      }
    }
  } else if (key == keyBindings.moveRight) {
    if (movesLeft) {
      if (me->entity->x < scene->width - 1) {
        me->entity->x++;
        draw(scene);
        if (sendMessage(keyBindings.moveRight) == -1)
          return ExitError;
        movesLeft--;
      }
    }
  } else if (key == keyBindings.quit) {
    addMessage("You quit the game. You lose!");
    drawHelpText(me, true, movesLeft);
    if (sendMessage(keyBindings.quit) == -1)
      return ExitError;
    return ExitOk;
  }
  return Nothing;
}

int main(int argc, char **argv)
{
  /* Set up the key bindings. */
  keyBindings.moveUp = 'k';
  keyBindings.moveRight = 'l';
  keyBindings.moveDown = 'j';
  keyBindings.moveLeft = 'h';
  keyBindings.endTurn = 'n';
  keyBindings.attack = 'a';
  keyBindings.quit = 'q';
  /* Set up the scene and players. */
  const int mapWidth = 80, mapLength = 40;
  /* Initialise the players. The player who waited for a connection begins in
   * the top left, the other player is in the bottom right. */
  rvlEntity *topLeftPlayer, *bottomRightPlayer;
  rvlError error;
  error = rvlEntityNewH(&topLeftPlayer, 0, 0, true, "\u263b");
  if (error != rvlNoError) {
    printf("main(): Could not allocate player object.\n");
    return EXIT_FAILURE;
  }
  error = rvlEntityNewH(&bottomRightPlayer, mapWidth - 1, mapLength - 1, true,
    "\u263a");
  if (error != rvlNoError) {
    printf("main(): Could not allocate player object.\n");
    free(topLeftPlayer);
    return EXIT_FAILURE;
  }
  if (argc == 2) { /* "Connecting" player. */
    error = rvlPlayerNewH(&me, bottomRightPlayer, 5, 5, 10);
    if (error != rvlNoError) {
      printf("main(): Could not allocate player object.\n");
      free(topLeftPlayer);
      free(bottomRightPlayer);
      return EXIT_FAILURE;
    }
    error = rvlPlayerNewH(&rival, topLeftPlayer, 5, 5, 10);
    if (error != rvlNoError) {
      printf("main(): Could not allocate player object.\n");
      free(bottomRightPlayer);
      rvlPlayerFree(me);
    }
    connectToServer(argv[1]);
  } else { /* "Waiting" player. */
    error = rvlPlayerNewH(&me, topLeftPlayer, 5, 5, 10);
    if (error != rvlNoError) {
      printf("main(): Could not allocate player object.\n");
      free(topLeftPlayer);
      free(bottomRightPlayer);
      return EXIT_FAILURE;
    }
    error = rvlPlayerNewH(&rival, bottomRightPlayer, 5, 5, 10);
    if (error != rvlNoError) {
      printf("main(): Could not allocate player object.\n");
      rvlPlayerFree(me);
      free(bottomRightPlayer);
      return EXIT_FAILURE;
    }
    if (startServer() == EXIT_FAILURE)
      return EXIT_FAILURE;
  }
  error = rvlSceneNewH(&scene, mapWidth, mapLength);
  if (error != rvlNoError) {
    printf("main(): Could not allocate scene object.\n");
    rvlPlayerFree(me);
    rvlPlayerFree(rival);
    return EXIT_FAILURE;
  }
  rvlSceneAddPlayer(scene, me); /* E. */
  rvlSceneAddPlayer(scene, rival);
  printf("[?25l");
  setbuf(stdout, NULL);
  printf("[2J");
  system("/bin/stty raw");
  struct termios oldTerminalSettings;
  tcgetattr(STDIN_FILENO, &oldTerminalSettings);
  oldTerminalSettings.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &oldTerminalSettings);
  char networkInput, userInput, messageBuffer[100] = {'\0'};
  draw(scene);
  /* Figuring out which player this is based on their position. Not great. */
  if (me->entity->x == scene->width - 1 && me->entity->y == scene->length - 1) {
    goto EndTurn;
  }
  while (true) {
    movesLeft = 5;
    while (true) {
      drawHelpText(me, true, movesLeft);
      userInput = getchar();
      if (userInput != '\n') {
        action act = keyPress(userInput);
        if (act == EndTurn)
          goto EndTurn;
        else if (act == ExitOk)
          goto CleanUpAndExit;
        else if (act == ExitError)
          goto CleanUpAndExitWithError;
      }
    }
EndTurn:
    drawHelpText(me, false, movesLeft);
    bool rivalTurn = true;
    while (rivalTurn) {
      if (getMessage(&networkInput) == -1) {
        goto CleanUpAndExitWithError;
      }
      switch (networkInput) {
      case 'a':
        attack(me, rival);
        draw(scene);
        addMessage("You have been attacked!");
        drawHelpText(me, false, movesLeft);
        break;
      case 'h':
        rival->entity->x--;
        draw(scene);
        break;
      case 'j':
        rival->entity->y++;
        draw(scene);
        break;
      case 'k':
        rival->entity->y--;
        draw(scene);
        break;
      case 'l':
        rival->entity->x++;
        draw(scene);
        break;
      case 'n':
        rivalTurn = false;
        break;
      case 'q':
        addMessage("Your opponent has quit. You win!");
        drawHelpText(me, false, movesLeft);
        goto CleanUpAndExit;
        break;
      }
    }
  }
  return EXIT_SUCCESS;
CleanUpAndExitWithError:
  perror("main()");
CleanUpAndExit:
  printf("[E");
  printf("[?25h");
  tcgetattr(STDIN_FILENO, &oldTerminalSettings);
  oldTerminalSettings.c_lflag |= ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &oldTerminalSettings);
  system("/bin/stty cooked");
  return closeNetworkResources();
}

