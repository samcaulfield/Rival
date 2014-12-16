#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <termios.h>
#include <unistd.h>

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

char *playerSkin, *rivalSkin;

void draw(int width, int length, int playerIndex, int rivalIndex)
{
  printf("[2J");
  printf("[1;1H");
  int i = 0, j, index;
  for (i; i < length; i++) {
    j = 0;
    for (j; j < width; j++) {
      index = i * width + j;
      if (index == playerIndex) {
        printf(playerSkin);
      } else if (index == rivalIndex) {
        printf(rivalSkin);
      } else {
        printf(".");
      }
    }
    printf("[E");
  }
}

int main(int argc, char **argv)
{
#define width 80
#define length 40
  int playerIndex, rivalIndex;
  if (argc == 2) {
    playerSkin = "\u263a";
    rivalSkin = "\u263b";
    playerIndex = width * length - 1;
    rivalIndex = 0;
    connectToServer(argv[1]);
  } else {
    if (startServer() == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }
    playerSkin = "\u263b";
    rivalSkin = "\u263a";
    playerIndex = 0;
    rivalIndex = width * length - 1;
  }
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
  draw(width, length, playerIndex, rivalIndex);
  if (playerIndex == (width * length - 1)) {
    goto EndTurn;
  }
  while (true) {
    printf("[EIt's your turn!\n");
    movesLeft = 5;
    while (true) {
      userInput = getchar();
      if (userInput != '\n') {
        switch (userInput) {
        case 'j':
          if (movesLeft) {
            if ((playerIndex < (length * (width - 1)))) {
              playerIndex += width;
              draw(width, length, playerIndex, rivalIndex);
              if (sendMessage('j') == -1) {
                goto CleanUpAndExitWithError;
              }
              movesLeft--;
            }
          }
          break;
         case 'k':
          if (movesLeft) {
            if (playerIndex > width) {
              playerIndex -= width;
              draw(width, length, playerIndex, rivalIndex);
              if (sendMessage('k') == -1) {
                goto CleanUpAndExitWithError;
              }
              movesLeft--;
            }
          }
          break;      
        case 'l':
	  if (movesLeft) {
	    if ((playerIndex % width) != width - 1) {
	      playerIndex++;
	      draw(width, length, playerIndex, rivalIndex);
	      if (sendMessage('l') == -1) {
		goto CleanUpAndExitWithError;
	      }
	      movesLeft--;
	    }
	  }
	  break;
        case 'h':
          if (movesLeft) {
            if ((playerIndex % width) > 0) {
              playerIndex--;
              draw(width, length, playerIndex, rivalIndex);
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
    printf("[EIt's the opponent's turn!\n");
    bool rivalTurn = true;
    while (rivalTurn) {
      if (getMessage(&networkInput) == -1) {
        goto CleanUpAndExitWithError;
      }
      switch (networkInput) {
      case 'h':
        rivalIndex--;
        draw(width, length, playerIndex, rivalIndex);
        break;
      case 'j':
        rivalIndex += width;
        draw(width, length, playerIndex, rivalIndex);
        break;
      case 'k':
        rivalIndex -= width;
        draw(width, length, playerIndex, rivalIndex);
        break;
      case 'l':
        rivalIndex++;
        draw(width, length, playerIndex, rivalIndex);
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

