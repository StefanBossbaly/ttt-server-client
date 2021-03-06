#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gameserver.h"
#include "game.h"
#include "player_record.h"

//Helper functions

//Handlers
static int gameserver_handle_disconnect(void *data, int id);
static int gameserver_handle_recieve(void *data, int socket_id, char *buffer, size_t size);

//Broadcast helpers
static void gameserver_broadcast_start(gameserver_t *gameserver);
static void gameserver_broadcast_end(gameserver_t *gameserver, int condition, player_t winner);
static void gameserver_send_error(gameserver_t *gameserver, int socket_id, int code);
static void gameserver_broadcast_move(gameserver_t *gameserver, int x, int y, player_t player);

void gameserver_init(gameserver_t *gameserver, subserver_t *subserver)
{
	//Assign
	gameserver->subserver = subserver;

	//Register the handler
	subserver_reg_data(subserver, gameserver);
	subserver_reg_rec_handler(subserver, gameserver_handle_recieve);
	subserver_reg_dis_handler(subserver, gameserver_handle_disconnect);

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

	//Add the subservers clients to our game
	for (i = 0; i < subserver->clients_size; i++)
	{
		gameserver_set_player(gameserver, subserver->clients[i], i + 1);
	}

	//Make sure all things are zero
	gameserver->player_size = subserver->clients_size;

	//Let our clients know that the game has started!
	gameserver_broadcast_start(gameserver);
}

void gameserver_handle(gameserver_t *gameserver)
{
	//Just call the handle function for our subserver
	subserver_handle(gameserver->subserver);
}

player_soc_t *gameserver_get_player(gameserver_t *gameserver, int socket_id)
{
	int i;
	for (i = 0; i < gameserver->player_size; i++)
	{
		if (gameserver->players[i].socket_id == socket_id)
		{
			return &gameserver->players[i];
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
	return ttt_is_end_of_game(gameserver->game) || gameserver->subserver->clients_size != 2;
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

//Broadcast helpers
void gameserver_broadcast_start(gameserver_t *gameserver)
{
	char command[] = "START";
	subserver_brodcast(gameserver->subserver, command, sizeof(command));
}

void gameserver_broadcast_end(gameserver_t *gameserver, int condition, player_t winner)
{
	//How many characters do we need?
	int length = snprintf(NULL, 0, "END %i %i", condition, (int) winner) + 1;

	//Allocate the space
	char *broadcast = (char *) malloc(length * sizeof(char));

	//Do the string concatenation
	sprintf(broadcast, "END %i %i", 0, winner);

	//TODO this is very bad, implement something better than busy waiting
	//int z;
	//for (z = 0; z < 100000000; z++) { };

	printf("Broadcasting command: %s\n", broadcast);

	//Send it to the client
	subserver_brodcast(gameserver->subserver, broadcast, length);

	free(broadcast);
}

void gameserver_send_error(gameserver_t *gameserver, int socket_id, int code)
{
	//How many characters do we need?
	int length = snprintf(NULL, 0, "ERROR %i", code) + 1;

	//Allocate the space
	char *broadcast = (char *) malloc(length * sizeof(char));

	//Do the string concatenation
	sprintf(broadcast, "ERROR %i", code);

	printf("Sending command: %s\n", broadcast);

	//Send it to the client
	subserver_send(gameserver->subserver, socket_id, broadcast, length);

	free(broadcast);
}

void gameserver_broadcast_move(gameserver_t *gameserver, int x, int y, player_t player)
{
	//How many characters do we need?
	int length = snprintf(NULL, 0, "MOVE %i %i %i", x, y, (int) player) + 1;

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

//Handlers
int gameserver_handle_disconnect(void *data, int id)
{
	gameserver_t *gameserver = (gameserver_t *) data;

	//Get the player that dc'ed
	player_t dc = gameserver_get_player(gameserver, id)->player;

	printf("Player %i disconnected. Ending the game\n", (int) dc);

	player_t winner;

	if (dc == PLAYER1)
	{
		winner = PLAYER2;
	}
	else
	{
		winner = PLAYER1;
	}

	gameserver_broadcast_end(gameserver, 1, winner);

	return 0;
}

int gameserver_handle_recieve(void *data, int socket_id, char *buffer, size_t size)
{
	printf("Game server received command: %s\n", buffer);

	gameserver_t *gameserver = (gameserver_t *) data;

	int x = 0, y = 0;
	char first_name[FIRST_NAME_MAX];
	char last_name[LAST_NAME_MAX];

	if (sscanf(buffer, "ID %i %s %s", &x, first_name, last_name))
	{
		//Get the calling player
		player_soc_t *player = gameserver_get_player(gameserver, socket_id);

		player->player_id = x;
		strcpy(player->first_name, first_name);
		strcpy(player->last_name, last_name);

		return 0;
	}
	else if (sscanf(buffer, "MOVE %i %i", &x, &y))
	{
		printf("MOVE COMMAND x = %i and y = %i\n", x, y);

		player_t player = gameserver_get_player(gameserver, socket_id)->player;

		printf("Player id as %i \n", player);

		//Make sure that we can move there
		if (gameserver->game->turn != player)
		{
			printf("Invalid move command. Not players turn\n");

			//Send out error code 1
			gameserver_send_error(gameserver, socket_id, 1);

			return -1;
		}

		//Make sure that the pos is in bounds
		if (!ttt_is_in_bounds(gameserver->game, x, y))
		{
			printf("Invalid move command. Pos is out of bounds\n");

			//Send out error code 2
			gameserver_send_error(gameserver, socket_id, 2);

			return -1;
		}

		//Make sure the position is not occupied
		if (ttt_is_pos_occupied(gameserver->game, x, y))
		{
			printf("Invalid move command. Pos is occupied\n");

			//Send out error code 2
			gameserver_send_error(gameserver, socket_id, 3);

			return -1;
		}

		printf("MOVE COMMAND valid\n");

		gameserver_broadcast_move(gameserver, x, y, player);

		//Check to see if it is end of game
		if (ttt_is_end_of_game(gameserver->game))
		{
			printf("End of game condition!\n");

			//Get the winner
			player_t winner = ttt_winner(gameserver->game);

			gameserver_broadcast_end(gameserver, 0, winner);
		}

		return 0;
	}

	return -1;
}
