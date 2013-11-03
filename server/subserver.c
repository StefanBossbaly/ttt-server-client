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

void subserver_reg_handler(subserver_t *subserver, int (*handle_recieve)(void *subserver, int id, char *buf, size_t size), void *data)
{
	subserver->handle_recieve = handle_recieve;
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

			//Call the handler
			subserver->handle_recieve(subserver->data, subserver->clients[i], buffer, sizeof(buffer));
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
