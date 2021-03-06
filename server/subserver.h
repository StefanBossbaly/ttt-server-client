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

	//Method handlers
	int (*handle_recieve)(void *data, int id, char *buf, size_t size);
	int (*handle_disconnect)(void *data, int id);
} subserver_t;

void subserver_init(subserver_t *subserver, int *clients, int size);
void subserver_reg_rec_handler(subserver_t *subserver, int (*handle_recieve)(void *subserver, int id, char *buf, size_t size));
void subserver_reg_dis_handler(subserver_t *subserver, int (*handle_disconnect)(void *data, int id));
void subserver_reg_data(subserver_t *subserver, void *data);
void subserver_handle(subserver_t *subserver);
void subserver_brodcast(subserver_t *subserver, const char *command, size_t size);
void subserver_send(subserver_t *subserver, int socket_id, const char *command, size_t size);
void subserver_add_client(subserver_t *subserver, int socket_id);
void subserver_remove_client(subserver_t *subserver, int socket_id);
int subserver_max_fd(subserver_t *subserver);
void subserver_close(subserver_t *subserver);
#endif /* SUBSERVER_H_ */
