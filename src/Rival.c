#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "../include/Network.h"
#include "../include/Scene.h"
#include "../include/Rendering.h"

int main(int argc, char **argv)
{
  struct Player me, rival;
  if (argc == 2) {
    me.skin = "\u263a";
    rival.skin = "\u263b";
    me.x = WIDTH - 1;
    me.y = LENGTH - 1;
    rival.x = rival.y = 0;
    connectToServer(argv[1]);
  } else {
    if (startServer() == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }
    me.skin = "\u263b";
    rival.skin = "\u263a";
    me.x = me.y = 0;
    rival.x = WIDTH - 1;
    rival.y = LENGTH - 1;
  }
  me.health = rival.health = 10;
  me.attack = rival.attack = 5;
  me.defence = rival.defence = 5;
  printf("[?25l");
  setbuf(stdout, NULL);
  printf("[2J");
  system("/bin/stty raw");
  struct termios oldTerminalSettings;
  tcgetattr(STDIN_FILENO, &oldTerminalSettings);
  oldTerminalSettings.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &oldTerminalSettings);
  int movesLeft;
  char networkInput, userInput, messageBuffer[100] = {'\0'},
    map[WIDTH * LENGTH];
  draw(WIDTH, LENGTH, me, rival);
  if (me.x == WIDTH - 1 && me.y == LENGTH - 1) {
    goto EndTurn;
  }
  while (true) {
    movesLeft = 5;
    while (true) {
      drawHelpText(me, true, movesLeft);
      userInput = getchar();
      if (userInput != '\n') {
        switch (userInput) {
        case 'a':
          if (movesLeft) {
            if (distance(me, rival) <= 1) {
              int meHp = me.health, rivalHp = rival.health;
              attack(&me, &rival);
              int meLost = me.health - meHp, rivalLost = rivalHp - rival.health;
              if (sendMessage('a') == -1) {
                goto CleanUpAndExitWithError;
              }
              draw(WIDTH, LENGTH, me, rival);
              movesLeft--;
              addMessage("Attack!");
            } else {
              addMessage("Nothing to attack.");
            }
          }
          break;
        case 'j':
          if (movesLeft) {
            if (me.y < LENGTH - 1) {
              me.y++;
              draw(WIDTH, LENGTH, me ,rival);
              if (sendMessage('j') == -1) {
                goto CleanUpAndExitWithError;
              }
              movesLeft--;
            }
          }
          break;
         case 'k':
          if (movesLeft) {
            if (me.y > 0) {
              me.y--;
              draw(WIDTH, LENGTH, me ,rival);
              if (sendMessage('k') == -1) {
                goto CleanUpAndExitWithError;
              }
              movesLeft--;
            }
          }
          break;      
        case 'l':
	  if (movesLeft) {
	    if (me.x < WIDTH - 1) {
	      me.x++;
	      draw(WIDTH, LENGTH, me ,rival);
	      if (sendMessage('l') == -1) {
		goto CleanUpAndExitWithError;
	      }
	      movesLeft--;
	    }
	  }
	  break;
        case 'h':
          if (movesLeft) {
            if (me.x > 0) {
              me.x--;
              draw(WIDTH, LENGTH, me ,rival);
              if (sendMessage('h') == -1) {
                goto CleanUpAndExitWithError;
              }
              movesLeft--;
            }
          }
          break;
	case 'n':
	  if (sendMessage('n') == -1) {
	    goto CleanUpAndExitWithError;
	  }
	  goto EndTurn;
	case 'q':
	  if (sendMessage('q') == -1) {
	    goto CleanUpAndExitWithError;
	  }
	  goto CleanUpAndExit;
	}
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
        attack(&me, &rival);
        draw(WIDTH, LENGTH, me, rival);
        addMessage("You have been attacked!");
        drawHelpText(me, false, movesLeft);
        break;
      case 'h':
        rival.x--;
        draw(WIDTH, LENGTH, me ,rival);
        break;
      case 'j':
        rival.y++;
        draw(WIDTH, LENGTH, me ,rival);
        break;
      case 'k':
        rival.y--;
        draw(WIDTH, LENGTH, me ,rival);
        break;
      case 'l':
        rival.x++;
        draw(WIDTH, LENGTH, me ,rival);
        break;
      case 'n':
        rivalTurn = false;
        break;
      case 'q':
        printf("[EYour opponent has quit. You win!\n[E");
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
  tcgetattr(STDIN_FILENO, &oldTerminalSettings);
  oldTerminalSettings.c_lflag |= ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &oldTerminalSettings);
  system("/bin/stty cooked");
  return closeNetworkResources();
}

