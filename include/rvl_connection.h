#ifndef rvl_connection_h
#define rvl_connection_h

#include <stdbool.h>

void rvl_connection_close();

bool rvl_connection_new(char *host);

char rvl_connection_recv();

bool rvl_connection_send(char msg);

#endif

