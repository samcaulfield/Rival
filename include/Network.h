#ifndef NETWORK_H
#define NETWORK_H

int connectToServer(char *host);

int startServer();

int getMessage(char *message);

int sendMessage(char message);

int closeNetworkResources();

#endif

