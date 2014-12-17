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
    me = NewPlayer(NewEntity(WIDTH - 1, LENGTH - 1, true, "\u263a"), 5, 5, 10);
    rival = NewPlayer(NewEntity(0, 0, true, "\u263b"), 5, 5, 10);
    connectToServer(argv[1]);
  } else {
    if (startServer() == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }
    me = NewPlayer(NewEntity(0, 0, true, "\u263b"), 5, 5, 10);
    rival = NewPlayer(NewEntity(WIDTH - 1, LENGTH - 1, true, "\u263a"), 5, 5,
      10);
  }
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
  if (me.entity.x == WIDTH - 1 && me.entity.y == LENGTH - 1) {
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
            if (me.entity.y < LENGTH - 1) {
              me.entity.y++;
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
            if (me.entity.y > 0) {
              me.entity.y--;
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
	    if (me.entity.x < WIDTH - 1) {
	      me.entity.x++;
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
            if (me.entity.x > 0) {
              me.entity.x--;
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
          addMessage("You quit the game. You lose!");
          drawHelpText(me, true, movesLeft);
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
        rival.entity.x--;
        draw(WIDTH, LENGTH, me ,rival);
        break;
      case 'j':
        rival.entity.y++;
        draw(WIDTH, LENGTH, me ,rival);
        break;
      case 'k':
        rival.entity.y--;
        draw(WIDTH, LENGTH, me ,rival);
        break;
      case 'l':
        rival.entity.x++;
        draw(WIDTH, LENGTH, me ,rival);
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

