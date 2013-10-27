#include "server.h"
#include "subserver.h"
#include "chatserver.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define HOST "localhost\0"
#define PORT 8080
#define BACKLOG 10
#define NUM_THREADS	5

void *thread_server_do_work(void *uncast_subserver)
{
	printf("Thread created!\n");

	subserver_t *subserver = (subserver_t *) uncast_subserver;
	chatserver_t *chatserver = (chatserver_t *) malloc(sizeof(chatserver_t));

	chatserver_init(chatserver, subserver);

	while (1)
	{
		chatserver_handle(chatserver);
	}

	return NULL;
}

int main()
{
	//Create server thread and subserver threads
	pthread_t subserver_threads[NUM_THREADS];
	int i = 0;
	pthread_attr_t attr;

	/* Initialize and set thread detached attribute */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	//Create a server and start listening
	server_t server;
	server_init(&server, "127.0.0.1", 3700, 10);
	server_start(&server);

	while (1)
	{
		server_handle(&server);

		if (server.client_size == 2)
		{
			printf("Creating thread!\n");

			subserver_t *subserver = (subserver_t *) malloc(sizeof(subserver_t));

			subserver_init(subserver, server.clients, 2);

			int resultCode = pthread_create(&subserver_threads[i], &attr, thread_server_do_work, subserver);

			if (resultCode)
			{
				printf("ERROR; return code from pthread_create() is %d\n", resultCode);
				exit(-1);
			}

			//Create a new thread
			server.client_size = 0;
			i++;
		}
	}

	return 0;
}
