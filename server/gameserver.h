#ifndef GAMESERVER_H_
#define GAMESERVER_H_

#include "game.h"
#include "subserver.h"

#define ROWS 3
#define COLUMNS 3
#define CONNECT 3
#define MAX_PLAYERS 10

typedef struct
{
	int socket_id;
	player_t player;
} player_soc_t;

typedef struct
{
	player_soc_t players[MAX_PLAYERS];
	int player_size;
	subserver_t *subserver;
	game_t *game;
} gameserver_t;

void gameserver_init(gameserver_t *chatserver, subserver_t *subserver);
void gameserver_handle(gameserver_t *chatserver);
player_t gameserver_get_player(gameserver_t *gameserver, int socket_id);
void gameserver_set_player(gameserver_t *gameserver, int socket_id, player_t player);
int gameserver_is_finished(gameserver_t *gameserver);
void gameserver_close(gameserver_t *gameserver);

#endif /* GAMESERVER_H_ */
