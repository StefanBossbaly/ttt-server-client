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
			//subserver->handle_recieve(subserver->clients[i], buffer, sizeof(buffer));

			int j;
			for (j = 0; j < subserver->clients_size; j++)
			{
				// send to everyone!
				if (FD_ISSET(subserver->clients[j], &subserver->master))
				{
					// except the listener and ourselves
					if (send(subserver->clients[j], buffer, nbytes, 0) == -1)
					{
						perror("send");
					}
				}

				printf("This was received: %s\n", buffer);
			}
		}
	}
}

void subserver_brodcast(subserver_t *subserver, const char *command, size_t size)
{

}
