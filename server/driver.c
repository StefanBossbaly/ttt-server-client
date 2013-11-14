#include "server.h"
#include "subserver.h"
#include "chatserver.h"
#include "gameserver.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <signal.h>

#define HOST "localhost\0"
#define PORT 8080
#define BACKLOG 10

/**
 * Command to connect: ssh bossbalys2@server1.cs.scranton.edu -L 32600:localhost:32600
 */

server_t *gameserver = NULL;

void chld_signal_handler(int signal)
{
	int status;
	wait(&status);

	printf("Child process is dead with return status %i\n", status);
}

void int_signal_handler(int signal)
{
	printf("Server terminating\n");

	if (gameserver != NULL)
	{
		server_close(gameserver);
	}

	free(gameserver);

	printf("Goodbye!\n");

	exit(EXIT_SUCCESS);
}

int main()
{
	//Register signal callback
	signal(SIGCHLD, chld_signal_handler);
	signal(SIGINT, int_signal_handler);


	/*if (fork() == 0)
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
	{*/
		gameserver = (server_t *) malloc(sizeof(server_t));
		server_init(gameserver, "192.168.1.7", 32600, 10);
		server_start(gameserver);

		while (1)
		{
			server_handle(gameserver);

			if (gameserver->client_size == 2)
			{
				subserver_t *subserver = (subserver_t *) malloc(sizeof(subserver_t));

				subserver_init(subserver, gameserver->clients, gameserver->client_size);

				if (fork() == 0)
				{
					printf("Creating game server\n");

					gameserver_t *gameserver = (gameserver_t *) malloc(sizeof(gameserver_t));

					gameserver_init(gameserver, subserver);

					while (! gameserver_is_finished(gameserver))
					{
						gameserver_handle(gameserver);
					}

					printf("Gameserver is shutting down\n");

					gameserver_close(gameserver);

					free(gameserver);

					exit(EXIT_SUCCESS);
				}
				else
				{
					int i;
					for (i = 0; i < gameserver->client_size; i++)
					{
						if (close(gameserver->clients[i]) == -1)
						{
							perror("Closing failed");
						}
					}

					gameserver->client_size = 0;
				}
			}
		}
	//}

	return 0;
}
