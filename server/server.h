#ifndef SERVER_H_
#define SERVER_H_

#include <sys/socket.h>

#define MAX_CLIENTS 10

typedef struct
{
	char host[100];
	int port;
	int backlog;
	int socket_id;
	struct sockaddr_storage clients[MAX_CLIENTS];

} server_t;

void server_init(server_t *server, char *host, int port, int backlog);
int server_start(server_t *server);
int server_accept_client(server_t *server);

#endif /* SERVER_H_ */
