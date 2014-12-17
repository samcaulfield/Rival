#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
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

int closeNetworkResources()
{
  bool error = false;
  if (close(clientSocketID) == -1) {
    perror("closeNetworkResources()");
    error = true;
  }
  if (close(serverSocketID) == -1) {
    perror("closeNetworkResources()");
    error = true;
  }
  return (error) ? EXIT_FAILURE : EXIT_SUCCESS;
}

