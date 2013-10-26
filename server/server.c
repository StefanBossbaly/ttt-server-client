#include "server.h"

#include <unistd.h>

#include <errno.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

void server_init(server_t *server, char *host, int port, int backlog)
{
	strcpy(server->host, host);
	server->port = port;
	server->backlog = backlog;
	server->client_size = 0;
	server->fdmax = 0;

	FD_ZERO(&server->master);    // clear the master and temp sets
	FD_ZERO(&server->read_fds);
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

		//Bind our socket to the port
		if (bind(server_socket, p->ai_addr, p->ai_addrlen) == -1)
		{
			printf("Error: Binding a socket failed: %s\n", strerror(errno));
			continue;
		}

		//Print out
		print_ip(servinfo);

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

		// add the listener to the master set
		FD_SET(server->socket_id, &server->master);

		server->fdmax = server->socket_id;

		return 0;
	}

	printf("Error: Could not create socket");
	exit(EXIT_FAILURE);

	return 0;
}

void server_handle(server_t *server)
{
	char remoteIP[INET6_ADDRSTRLEN];

	//Create a temp version of our socket list
	server->read_fds = server->master;

	if (select(server->fdmax + 1, &server->read_fds, NULL, NULL, NULL) == -1)
	{
		perror("select");
		exit(EXIT_FAILURE);
	}

	int i;
	for (i = 0; i <= server->fdmax; i++)
	{
		//We found a socket that needs some processing time
		if (FD_ISSET(i, &server->read_fds))
		{
			//The listener has a new request
			if (i == server->socket_id)
			{
				socklen_t sin_size = sizeof(struct sockaddr_storage);
				struct sockaddr_storage client_addr;

				//Accept the new request
				int newFd = accept(server->socket_id, (struct sockaddr *) &client_addr, &sin_size);
				if (newFd == -1)
				{
					printf("Error: Accepting a new client failed: %s\n", strerror(errno));
					continue;
				}

				FD_SET(newFd, &server->master); // add to master set
				if (newFd > server->fdmax)
				{    // keep track of the max
					server->fdmax = newFd;
				}
				printf("selectserver: new connection from %s on "
						"socket %d\n", inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr*) &client_addr), remoteIP, INET6_ADDRSTRLEN), newFd);
			}
			else
			{
			    char buf[256];    // buffer for client data
			    int nbytes;

				// handle data from a client
				if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0)
				{
					// got error or connection closed by client
					if (nbytes == 0)
					{
						// connection closed
						printf("selectserver: socket %d hung up\n", i);
					}
					else
					{
						perror("recv");
					}

					close(i); // bye!
					FD_CLR(i, &server->master); // remove from master set
				}
				else
				{
					// we got some data from a client
					int j = 0;
					for (j = 0; j <= server->fdmax; j++)
					{
						// send to everyone!
						if (FD_ISSET(j, &server->master))
						{
							// except the listener and ourselves
							if (j != server->socket_id)
							{
								if (send(j, buf, nbytes, 0) == -1)
								{
									perror("send");
								}
							}
						}
					}
				}
			}
		}
	}
}

int server_accept_client(server_t *server)
{
	int reply_sock_fd = -1;
	socklen_t sin_size = sizeof(struct sockaddr_storage);
	struct sockaddr_storage client_addr;

	if ((reply_sock_fd = accept(server->socket_id, (struct sockaddr *) &client_addr, &sin_size)) == -1)
	{
		printf("socket accept error\n");
		return -1;
	}

	server->clients[server->client_size] = reply_sock_fd;
	server->client_size++;

	char client_printable_addr[INET6_ADDRSTRLEN];
	inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *) &client_addr), client_printable_addr, sizeof client_printable_addr);
	printf("server: connection from %s at port %d\n", client_printable_addr, ((struct sockaddr_in*) &client_addr)->sin_port);

	return 0;
}

void print_ip(struct addrinfo *ai)
{
	struct addrinfo *p;
	void *addr;
	char *ipver;
	char ipstr[INET6_ADDRSTRLEN];
	struct sockaddr_in *ipv4;
	struct sockaddr_in6 *ipv6;
	short port = 0;

	for (p = ai; p != NULL; p = p->ai_next)
	{
		if (p->ai_family == AF_INET)
		{
			ipv4 = (struct sockaddr_in *) p->ai_addr;
			addr = &(ipv4->sin_addr);
			port = ipv4->sin_port;
			ipver = "IPV4";
		}
		else
		{
			ipv6 = (struct sockaddr_in6 *) p->ai_addr;
			addr = &(ipv6->sin6_addr);
			port = ipv4->sin_port;
			ipver = "IPV6";
		}

		inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
		printf("serv ip info: %s - %s @%d\n", ipstr, ipver, ntohs(port));
	}
}

void *get_in_addr(struct sockaddr * sa)
{
	if (sa->sa_family == AF_INET)
	{
		printf("ipv4\n");
		return &(((struct sockaddr_in *) sa)->sin_addr);
	}
	else
	{
		printf("ipv6\n");
		return &(((struct sockaddr_in6 *) sa)->sin6_addr);
	}
}

