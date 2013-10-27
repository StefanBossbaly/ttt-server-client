#include "server.h"
#include "subserver.h"
#include "chatserver.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define HOST "localhost\0"
#define PORT 8080
#define BACKLOG 10

int main()
{
	if (fork() == 0)
	{
		server_t *chatserver = (server_t *) malloc(sizeof(server_t));
		server_init(chatserver, "127.0.0.1", 3701, 10);
		server_start(chatserver);

		while (1)
		{
			server_handle(chatserver);

			if (chatserver->client_size == 2)
			{
				subserver_t *subserver = (subserver_t *) malloc(sizeof(subserver_t));

				subserver_init(subserver, chatserver->clients, chatserver->client_size);

				if (fork() == 0)
				{
					chatserver_t *chatserver = (chatserver_t *) malloc(sizeof(chatserver_t));

					chatserver_init(chatserver, subserver);

					while (1)
					{
						chatserver_handle(chatserver);
					}
				}
				else
				{
					chatserver->client_size = 0;
				}
			}
		}
	}
	else
	{
		server_t *gameserver = (server_t *) malloc(sizeof(server_t));
		server_init(gameserver, "127.0.0.1", 3700, 10);
		server_start(gameserver);

		while (1)
		{
			server_handle(gameserver);
		}
	}

	return 0;
}
