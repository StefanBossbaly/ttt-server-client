/*
 * subserver.h
 *
 *  Created on: Oct 26, 2013
 *      Author: stefan
 */

#ifndef SUBSERVER_H_
#define SUBSERVER_H_

#define MAX_CLIENTS 10

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

typedef struct
{
	//List of the fd of sockets for our clients
	int clients[MAX_CLIENTS];

	//How many we have
	int clients_size;

	fd_set master;
	void *data;
	int (*handle_recieve)(void *data, int id, char *buf, size_t size);
} subserver_t;

void subserver_init(subserver_t *subserver, int *clients, int size);
void subserver_reg_handler(subserver_t *subserver, int (*handle_recieve)(void *subserver, int id, char *buf, size_t size), void *data);
void subserver_handle(subserver_t *subserver);
void subserver_brodcast(subserver_t *subserver, const char *command, size_t size);
void subserver_send(subserver_t *subserver, int socket_id, const char *command, size_t size);
int subserver_max_fd(subserver_t *subserver);
#endif /* SUBSERVER_H_ */
