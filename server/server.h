#ifndef SERVER_H_
#define SERVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define MAX_CLIENTS 10

typedef struct
{
	char host[100];
	int port;
	int backlog;
	int socket_id;
	int clients[MAX_CLIENTS];
	int client_size;
	fd_set master;    // master file descriptor list

} server_t;

void server_init(server_t *server, char *host, int port, int backlog);
int server_start(server_t *server);
void server_handle(server_t *server);
void server_close(server_t *server);

//debug methods
void print_ip(struct addrinfo *ai);
void *get_in_addr(struct sockaddr * sa);

#endif /* SERVER_H_ */
