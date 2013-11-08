#include "subserver.h"

#include <unistd.h>

#include <errno.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

void subserver_init(subserver_t *subserver, int *clients, int size)
{
	subserver->handle_recieve = NULL;
	subserver->handle_disconnect = NULL;

	FD_ZERO(&subserver->master);

	int i;
	for (i = 0; i < size; i++)
	{
		//Add the client to our list of clients
		subserver->clients[i] = clients[i];

		//Add it our file descriptor set
		FD_SET(subserver->clients[i], &subserver->master);
	}

	subserver->clients_size = size;
}

void subserver_reg_rec_handler(subserver_t *subserver, int (*handle_recieve)(void *subserver, int id, char *buf, size_t size))
{
	subserver->handle_recieve = handle_recieve;
}

void subserver_reg_dis_handler(subserver_t *subserver, int (*handle_disconnect)(void *data, int id))
{
	subserver->handle_disconnect = handle_disconnect;
}

void subserver_reg_data(subserver_t *subserver, void *data)
{
	subserver->data = data;
}

int subserver_max_fd(subserver_t *subserver)
{
	int max = -1;

	int i;
	for (i = 0; i < subserver->clients_size; i++)
	{
		if (subserver->clients[i] > max)
		{
			max = subserver->clients[i];
		}
	}

	return max;
}

void subserver_handle(subserver_t *subserver)
{
	fd_set temp;

	//Copy our master to our temp
	memcpy(&temp, &subserver->master, sizeof(subserver->master));

	//Let's see has unhandled commands
	if (select(subserver_max_fd(subserver) + 1, &temp, NULL, NULL, NULL) == -1)
	{
		printf("Error: Subserver select failed!: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	int i;
	for (i = 0; i < subserver->clients_size; i++)
	{
		//We found a socket that needs some processing time
		if (FD_ISSET(subserver->clients[i], &temp))
		{
			//Buffer for receiving a command
			char buffer[1024];

			//TODO handle a close connection
			int nbytes = recv(subserver->clients[i], buffer, sizeof(buffer), 0);

			if (nbytes == -1)
			{
				perror("Error: Receive failed!");
				return;
			}

			//We had a disconnect
			if (nbytes == 0)
			{
				if (subserver->handle_disconnect != NULL)
				{
					subserver->handle_disconnect(subserver->data, subserver->clients[i]);
				}
				else
				{
					printf("Handle disconnect not registered\n");
				}

				subserver_remove_client(subserver, subserver->clients[i]);

				return;
			}

			//Call the handler
			if (subserver->handle_recieve != NULL)
			{
				subserver->handle_recieve(subserver->data, subserver->clients[i], buffer, sizeof(buffer));
			}
			else
			{
				printf("Handle Receive not registered\n");
			}
		}
	}
}

void subserver_add_client(subserver_t *subserver, int socket_id)
{
	if (subserver->clients_size == MAX_CLIENTS)
	{
		printf("Can't add client. Limit reached!\n");
		return;
	}

	FD_SET(socket_id, &subserver->master);

	subserver->clients[subserver->clients_size] = socket_id;
	subserver->clients_size++;
}

void subserver_remove_client(subserver_t *subserver, int socket_id)
{
	int location = -1;

	int i;
	for (i = 0; i < subserver->clients_size; i++)
	{
		if (subserver->clients[i] == socket_id)
		{
			location = i;
			break;
		}
	}

	//Make sure the socket is managed by us
	if (location != -1)
	{
		//Remove from the client list
		for (i = location; i < subserver->clients_size - 1; i++)
		{
			subserver->clients[i] = subserver->clients[i + 1];
		}
		subserver->clients_size--;

		FD_CLR(socket_id, &subserver->master);

		//Close the socket
		if (close(socket_id) == -1)
		{
			perror("Closing connection failed");
		}
	}
}

void subserver_brodcast(subserver_t *subserver, const char *command, size_t size)
{
	int i;
	for (i = 0; i < subserver->clients_size; i++)
	{
		if (send(subserver->clients[i], command, size, 0) == -1)
		{
			perror("subserver_brodcast");
		}
	}
}

void subserver_send(subserver_t *subserver, int socket_id, const char *command, size_t size)
{
	if (send(socket_id, command, size, 0) == -1)
	{
		perror("subserver_send");
	}
}

void subserver_close(subserver_t *subserver)
{
	int i;
	for (i = 0; i < subserver->clients_size; i++)
	{
		if (close(subserver->clients[i]) == -1)
		{
			perror("subserver_close");
		}
	}
}
