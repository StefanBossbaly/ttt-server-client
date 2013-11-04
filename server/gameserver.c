#include <stdlib.h>
#include <stdio.h>

#include "gameserver.h"
#include "game.h"

int gameserver_handle_recieve(void *data, int id, char *buffer, size_t size)
{
	printf("Game server received command: %s\n", buffer);

	gameserver_t *gameserver = (gameserver_t *) data;

	int x = 0, y = 0;

	if (sscanf(buffer, "MOVE %i %i", &x, &y))
	{
		printf("MOVE COMMAND x = %i and y = %i\n", x, y);

		player_t player = gameserver_get_player(gameserver, id);

		printf("Player id as %i \n", player);

		//Make sure that we can move there
		if (gameserver->game->turn != player)
		{
			printf("Invalid move command. Not players turn\n");

			//How many characters do we need?
			int length = snprintf(NULL, 0, "ERROR 1") + 1;

			//Allocate the space
			char *broadcast = (char *) malloc(length * sizeof(char));

			//Do the string concatenation
			sprintf(broadcast, "ERROR 1");

			printf("Sending command: %s\n", broadcast);

			//Send it to the client
			subserver_send(gameserver->subserver, id, broadcast, length);

			free(broadcast);

			return -1;
		}

		//TODO check error conditions

		printf("MOVE COMMAND valid\n");

		//How many characters do we need?
		int length = snprintf(NULL, 0, "MOVE %i %i %i", x, y, player) + 1;

		//Allocate the space
		char *broadcast = (char *) malloc(length * sizeof(char));

		//Do the string concatenation
		sprintf(broadcast, "MOVE %i %i %i", x, y, player);

		printf("Broadcasting command: %s\n", broadcast);

		//Send it to the client
		subserver_brodcast(gameserver->subserver, broadcast, length);

		//Make the move on the server
		ttt_make_move(gameserver->game, x, y);

		free(broadcast);
	}

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

	for (i = 0; i < subserver->clients_size; i++)
	{
		gameserver_set_player(gameserver, subserver->clients[i], i + 1);
	}

	//Make sure all things are zero
	gameserver->player_size = subserver->clients_size;
}

void gameserver_handle(gameserver_t *gameserver)
{
	//Just call the handle function for our subserver
	subserver_handle(gameserver->subserver);
}

player_t gameserver_get_player(gameserver_t *gameserver, int socket_id)
{
	int i;
	for (i = 0; i < gameserver->player_size; i++)
	{
		if (gameserver->players[i].socket_id == socket_id)
		{
			return gameserver->players[i].player;
		}
	}

	return NEITHER;
}

void gameserver_set_player(gameserver_t *gameserver, int socket_id, player_t player)
{
	gameserver->players[gameserver->player_size].socket_id = socket_id;
	gameserver->players[gameserver->player_size].player = player;

	gameserver->player_size++;
}
