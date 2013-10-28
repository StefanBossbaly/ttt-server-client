#include <stdlib.h>

#include "gameserver.h"
#include "game.h"

int gameserver_handle_recieve(void *data, int id, char *buffer, size_t size)
{
	gameserver_t *gameserver = (gameserver_t *) data;

	return 0;
}

void gameserver_init(gameserver_t *gameserver, subserver_t *subserver)
{
	//Assign
	gameserver->subserver = subserver;

	//Register the handler
	subserver_reg_handler(subserver, gameserver_handle_recieve, gameserver);

	//Allocate some space for our game
	gameserver->game = (game_t *) malloc(sizeof(game_t));

	//Allocate space for the grid
	cell_t **grid = (cell_t **) calloc(ROWS, sizeof(cell_t *));

	int i;
	for (i = 0; i < ROWS; i++)
	{
		grid[i] = calloc(COLUMNS, sizeof(cell_t));
	}

	//Init our game
	ttt_init_game(gameserver->game, ROWS, COLUMNS, CONNECT, grid);
}

void gameserver_handle(gameserver_t *chatserver)
{

}
