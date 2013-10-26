#include "server.h"

#include <stdio.h>

#define HOST "localhost\0"
#define PORT 8080
#define BACKLOG 10

int main()
{
	server_t server;

	server_init(&server, "127.0.0.1", 8080, 10);

	server_start(&server);

	printf("Socket ID: %i\n", server.socket_id);

	while(1)
	{
		server_handle(&server);
	}

	return 0;
}
