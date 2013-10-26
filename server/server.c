#include "server.h"

#include <errno.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

void server_init(server_t *server, char *host, int port, int backlog)
{
	strcpy(server->host, host);
	server->port = port;
	server->backlog = backlog;
}

int server_start(server_t *server)
{
	int status = -1, server_socket = -1;
	int yes = 1;
	struct addrinfo hints, *servinfo;

	//Make sure hint is filled with all zeros
	memset(&hints, 0, sizeof(hints));

	//Accept IPv4 or IPv6
	hints.ai_family = AF_UNSPEC;
	//Use TCP for transport layer
	hints.ai_socktype = SOCK_STREAM;
	//Allow a null node
	hints.ai_flags = AI_PASSIVE;

	//Have to convert port from an integer to a char *
	char port[20];
	sprintf(port, "%d", server->port);

	//Do a DNS look up and store the server information into servinfo
	if ((status = getaddrinfo(server->host, port, &hints, &servinfo)) != 0)
	{
		printf("Error: getaddrinfo() failed with error: %s\n", gai_strerror(status));
		exit(EXIT_FAILURE);
	}

	//Iterate through all the results that we receive
	//Once the bind call works this loop will exit
	struct addrinfo *p;
	for (p = servinfo; p != NULL; p = p->ai_next)
	{

		//Create a socket
		if ((server_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			printf("Error: Creating a socket failed: %s\n", strerror(errno));
			continue;
		}

		//Make sure that the port is released
		if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
			printf("Error: Releasing a socket failed: %s\n", strerror(errno));
			continue;
		}

		//Bind our socket to our socket to our connection
		if (bind(server_socket, p->ai_addr, p->ai_addrlen) == -1)
		{
			printf("Error: Binding a socket failed: %s\n", strerror(errno));
			continue;
		}

		//Free the server information structure
		freeaddrinfo(servinfo);

		//Set our socket id to the server_socket
		server->socket_id = server_socket;

		//Start listing for connections
		if (listen(server->socket_id, server->backlog) == -1)
		{
			printf("Error: Server listing failed: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		return 0;
	}

	printf("Error: Could not create socket");
	exit(EXIT_FAILURE);

	return 0;
}

int server_accept_client(server_t *server)
{
	return 0;
}

