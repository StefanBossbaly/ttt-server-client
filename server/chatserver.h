/*
 * chatserver.h
 *
 *  Created on: Oct 27, 2013
 *      Author: stefan
 */

#ifndef CHATSERVER_H_
#define CHATSERVER_H_

#define ALIAS_LEN 50

#include "subserver.h"

typedef struct
{
	int socket_id;
	char alias[ALIAS_LEN];
} alias_t;

typedef struct
{
	subserver_t *subserver;
	alias_t alias[10];
} chatserver_t;

void chatserver_init(chatserver_t *chatserver, subserver_t *subserver);
void chatserver_get_alias(chatserver_t *chatserver, int id, char *buffer);
void chatserver_set_alias(chatserver_t *chatserver, int id, char *alias);
void chatserver_handle(chatserver_t *chatserver);

#endif /* CHATSERVER_H_ */
