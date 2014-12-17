#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <termios.h>
#include <unistd.h>

struct Player {
  int attack, defence, health, x, y;
  char *skin;
};

struct sockaddr clientAddress;
int clientSocketID, clientAddressSize, serverSocketID;

int connectToServer(char *host)
{
  clientSocketID = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocketID == -1) {
    perror("connectToServer(char *)");
    return EXIT_FAILURE;
  }
  struct sockaddr_in serverAddress;
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = inet_addr(host);
  serverAddress.sin_port = htons(11235);
  if (connect(clientSocketID, (struct sockaddr *) &serverAddress,
    sizeof(serverAddress)) == -1) {
    perror("connectToServer(char *)");
    if (close(clientSocketID) == -1) {
      perror("connectToServer(char *)");
    }
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int startServer()
{
  serverSocketID = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocketID == -1) {
    return EXIT_FAILURE;
  }
  const int socketOption = true;
  if (setsockopt(serverSocketID, SOL_SOCKET, SO_REUSEADDR,
    (void *) &socketOption, sizeof(socketOption)) == -1) {
    goto CleanUpAndExit;
  }
  struct sockaddr_in serverAddress;
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddress.sin_port=htons(11235);
  if (bind(serverSocketID, (struct sockaddr *) &serverAddress,
    sizeof(serverAddress)) == -1) {
    goto CleanUpAndExit;
  }
  if (listen(serverSocketID, 1024) == -1) {
    goto CleanUpAndExit;
  }
  clientAddressSize = sizeof(clientAddress);
  clientSocketID = accept(serverSocketID,
    (struct sockaddr *) &clientAddress, &clientAddressSize);
  if (clientSocketID == -1) {
    goto CleanUpAndExit;
  }
  return EXIT_SUCCESS;
CleanUpAndExit:
  perror("startServer()");
  if (close(serverSocketID) == -1) {
    perror("startServer()");
  }
  return EXIT_FAILURE;
}

int getMessage(char *message)
{
  return recvfrom(clientSocketID, message, 1, 0,
      (struct sockaddr *) &clientAddress, &clientAddressSize);
}

int sendMessage(char message)
{
  return sendto(clientSocketID, &message, 1, 0, 
    (struct sockaddr *) &clientAddress, clientAddressSize);
}

void draw(int width, int length, struct Player me, struct Player rival)
{
  printf("[1;1H");
  int y = 0, x, index;
  for (y; y < length; y++) {
    x = 0;
    for (x; x < width; x++) {
      if (x == me.x && y == me.y) {
        printf(me.skin);
      } else if (x == rival.x && y == rival.y) {
        printf(rival.skin);
      } else {
        printf(".");
      }
    }
    printf("[E");
  }
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
}

int main(int argc, char **argv)
{
#define width 80
#define length 40
  struct Player me, rival;
  if (argc == 2) {
    me.skin = "\u263a";
    rival.skin = "\u263b";
    me.x = width - 1;
    me.y = length - 1;
    rival.x = rival.y = 0;
    connectToServer(argv[1]);
  } else {
    if (startServer() == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }
    me.skin = "\u263b";
    rival.skin = "\u263a";
    me.x = me.y = 0;
    rival.x = width - 1;
    rival.y = length - 1;
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
    map[width * length];
  draw(width, length, me, rival);
  if (me.x == width - 1 && me.y == length - 1) {
    goto EndTurn;
  }
  while (true) {
    movesLeft = 5;
    while (true) {
      drawHelpText(me, true, movesLeft);
      userInput = getchar();
      if (userInput != '\n') {
        switch (userInput) {
        case 'j':
          if (movesLeft) {
            if (me.y < length - 1) {
              me.y++;
              draw(width, length, me ,rival);
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
              draw(width, length, me ,rival);
              if (sendMessage('k') == -1) {
                goto CleanUpAndExitWithError;
              }
              movesLeft--;
            }
          }
          break;      
        case 'l':
	  if (movesLeft) {
	    if (me.x < width - 1) {
	      me.x++;
	      draw(width, length, me ,rival);
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
              draw(width, length, me ,rival);
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
      case 'h':
        rival.x--;
        draw(width, length, me ,rival);
        break;
      case 'j':
        rival.y++;
        draw(width, length, me ,rival);
        break;
      case 'k':
        rival.y--;
        draw(width, length, me ,rival);
        break;
      case 'l':
        rival.x++;
        draw(width, length, me ,rival);
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
  if (close(clientSocketID) == -1) {
    perror("main()");
    return EXIT_FAILURE;
  }
  if (close(serverSocketID) == -1) {
    perror("main()");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

