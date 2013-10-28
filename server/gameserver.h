#ifndef GAMESERVER_H_
#define GAMESERVER_H_

#include "game.h"
#include "subserver.h"

#define ROWS 3
#define COLUMNS 3
#define CONNECT 3


typedef struct
{
	subserver_t *subserver;
	game_t *game;
} gameserver_t;

void gameserver_init(gameserver_t *chatserver, subserver_t *subserver);
void gameserver_handle(gameserver_t *chatserver);


#endif /* GAMESERVER_H_ */
