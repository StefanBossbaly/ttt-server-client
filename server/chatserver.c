#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "chatserver.h"

int chatserver_handle_recieve(void *data, int id, char *buffer, size_t size)
{
	//Do a cast
	chatserver_t *chatserver = (chatserver_t *) data;
	subserver_t *subserver = chatserver->subserver;

	//The buffer for our message
	char message[255];
	char alias[51];

	//See if buffer matches our broadcast command
	if (sscanf(buffer, "BROADCAST %[^\n]", message))
	{
		printf("Broadcast called. Message is %s\n", message);

		chatserver_get_alias(chatserver, id, alias);

		printf("Broadcast called. Alias is %s\n", alias);

		//How many characters do we need?
		int length = snprintf(NULL, 0, "MESSAGE %s %s", alias, message) + 1;

		//Allocate the space
		char *broadcast = (char *) malloc(length * sizeof(char));

		//Do the string concatenation
		sprintf(broadcast, "MESSAGE %s %s\n", alias, message);

		//Send it to all of our clients
		subserver_brodcast(subserver, broadcast, length);

		free(broadcast);

		return 1;
	}
	else if (sscanf(buffer, "ALIAS %[^\n]", alias))
	{
		printf("Alias called. Alias is %s\n", alias);

		chatserver_set_alias(chatserver, id, alias);
	}

	return -1;
}

void chatserver_init(chatserver_t *chatserver, subserver_t *subserver)
{
	chatserver->subserver = subserver;

	//Register for callbacks
	subserver_reg_handler(subserver, chatserver_handle_recieve, chatserver);

	int i;
	for (i = 0; i < subserver->clients_size; i++)
	{
		//Create an alias for each person in our chat server
		chatserver->alias[i].socket_id = subserver->clients[i];
		strcpy(chatserver->alias[i].alias, "Default Player");
	}
}

void chatserver_get_alias(chatserver_t *chatserver, int id, char *buffer)
{
	int i;
	for (i = 0; i < chatserver->subserver->clients_size; i++)
	{
		if (chatserver->alias[i].socket_id == id)
		{
			strcpy(buffer, chatserver->alias[i].alias);
			return;
		}
	}
}

void chatserver_set_alias(chatserver_t *chatserver, int id, char *alias)
{
	int i;
	for (i = 0; i < chatserver->subserver->clients_size; i++)
	{
		if (chatserver->alias[i].socket_id == id)
		{
			strcpy(chatserver->alias[i].alias, alias);
			return;
		}
	}
}

void chatserver_handle(chatserver_t *chatserver)
{
	//Just call the handle function for our subserver
	subserver_handle(chatserver->subserver);
}
