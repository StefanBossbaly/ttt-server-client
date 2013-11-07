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

		//Make sure that the pos is in bounds
		if (!ttt_is_in_bounds(gameserver->game, x, y))
		{
			printf("Invalid move command. Pos is out of bounds\n");

			//How many characters do we need?
			int length = snprintf(NULL, 0, "ERROR 2") + 1;

			//Allocate the space
			char *broadcast = (char *) malloc(length * sizeof(char));

			//Do the string concatenation
			sprintf(broadcast, "ERROR 2");

			printf("Sending command: %s\n", broadcast);

			//Send it to the client
			subserver_send(gameserver->subserver, id, broadcast, length);

			free(broadcast);

			return -1;
		}

		//Make sure the position is not occupied
		if (ttt_is_pos_occupied(gameserver->game, x, y))
		{
			printf("Invalid move command. Pos is out of bounds\n");

			//How many characters do we need?
			int length = snprintf(NULL, 0, "ERROR 3") + 1;

			//Allocate the space
			char *broadcast = (char *) malloc(length * sizeof(char));

			//Do the string concatenation
			sprintf(broadcast, "ERROR 3");

			printf("Sending command: %s\n", broadcast);

			//Send it to the client
			subserver_send(gameserver->subserver, id, broadcast, length);

			free(broadcast);

			return -1;
		}

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

		//Check to see if it is end of game
		if (ttt_is_end_of_game(gameserver->game))
		{
			printf("End of game condition!\n");

			//Get the winner
			player_t winner = ttt_winner(gameserver->game);

			//How many characters do we need?
			length = snprintf(NULL, 0, "END %i %i", 1, (int) winner) + 1;

			//Allocate the space
			broadcast = (char *) malloc(length * sizeof(char));

			//Do the string concatenation
			sprintf(broadcast, "END %i %i", 0, winner);

			//TODO this is very bad, implement something better than busy waiting
			int z;
			for (z = 0; z < 100000000; z++) { };

			printf("Broadcasting command: %s\n", broadcast);

			//Send it to the client
			subserver_brodcast(gameserver->subserver, broadcast, length);

			free(broadcast);
		}

		return 0;
	}

	return -1;
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

int gameserver_is_finished(gameserver_t *gameserver)
{
	return ttt_is_end_of_game(gameserver->game);
}

void gameserver_close(gameserver_t *gameserver)
{
	//Close the subserver first
	subserver_close(gameserver->subserver);

	//Dealloc memory for game board
	int i;
	for (i = 0; i < ROWS; i++)
	{
		free(gameserver->game->grid[i]);
	}

	free(gameserver->game->grid);
}
