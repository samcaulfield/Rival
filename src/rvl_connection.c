#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "rvl_connection.h"

typedef enum {
        client, server
} connection_type;

static connection_type type;

static const int port = 11235;

static int server_id, client_id, client_size;

static struct sockaddr client_addr;

static bool start_client(char *host)
{
        client_id = socket(AF_INET, SOCK_STREAM, 0);
        if (client_id == -1)
                return false;
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(host);
        addr.sin_port = htons(port);
        if (connect(client_id, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
                close(client_id);
                return false;
        }
        type = client;
        return true;
}

static bool start_server()
{
        server_id = socket(AF_INET, SOCK_STREAM, 0);
        if (server_id == -1)
                return false;
        const int option = true;
        if (setsockopt(server_id, SOL_SOCKET, SO_REUSEADDR, (void *) &option,
                sizeof(option)) == -1)
                goto clean_up;
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);
        if (bind(server_id, (struct sockaddr *) &addr, sizeof(addr)) == -1)
                goto clean_up;
        if (listen(server_id, 1024) == -1)
                goto clean_up;
        client_size = sizeof(client_addr);
        client_id = accept(server_id, (struct sockaddr *) &client_addr,
                &client_size);
        if (client_id == -1)
                goto clean_up;
        type = server;
        return true;
clean_up:
        close(server_id);
        return false;
}

void rvl_connection_close()
{
        close((type == server) ? server_id : client_id);
}

bool rvl_connection_new(char *host)
{
        return (host == NULL) ? start_server() : start_client(host);
}

char rvl_connection_recv()
{
        char in;
        return ((recvfrom(client_id, &in, 1, 0, (struct sockaddr *) &client_addr,
                &client_size) == -1) ? '\0' : in);
        
}

bool rvl_connection_send(char msg)
{
       return sendto(client_id, &msg, 1, 0, (struct sockaddr *) &client_addr,
               client_size);
}

