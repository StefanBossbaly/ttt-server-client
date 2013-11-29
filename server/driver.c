#include "server.h"
#include "subserver.h"
#include "chatserver.h"
#include "gameserver.h"
#include "player_record.h"
#include "indexed_file.h"
#include "util/semaphore.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>

#define HOST "localhost\0"
#define PORT 8080
#define BACKLOG 10

const char *key = "/home/stefan/git/ttt/server/tttserver.o\0";

/**
 * Command to connect: ssh bossbalys2@server1.cs.scranton.edu -L 32600:localhost:32600
 */

server_t *gameserver = NULL;
indexed_file_t *database = NULL;

void chld_signal_handler(int signal)
{
	int status;
	wait(&status);

	printf("Child process is dead with return status %i\n", status);
}

void int_signal_handler(int signal)
{
	printf("Server terminating\n");

	if (gameserver != NULL)
	{
		server_close(gameserver);
	}

	free(gameserver);
	free(database);

	printf("Goodbye!\n");

	exit(EXIT_SUCCESS);
}

void print_player_record(player_record_t *record)
{
	printf("Record ID: %i\n", record->player_id);
	printf("Record Name: %s %s\n", record->first_name, record->last_name);
	printf("Recoard Stats %i-%i-%i\n", record->wins, record->ties, record->losses);
}

void save_draw(gameserver_t *gameserver, indexed_file_t *file)
{
	int i;
	for (i = 0; i < gameserver->player_size; i++)
	{
		int player_id = gameserver->players[i].player_id;

		//See if the index exists if it doesn't create it
		if (index_get_index(file, player_id) == -1)
		{
			//Create the record
			player_record_t record;

			//TODO have user tell us who he is
			player_rec_init(&record, player_id, "Joe", "Smith");

			//Set the number of ties to 1
			record.ties = 1;

			printf("Adding record .... \n");
			print_player_record(&record);

			//Add the record to the database
			index_add(file, player_id, &record);
		}
		else
		{
			//Create a buffer
			player_record_t record;

			//Load the record from the database to the buffer
			index_get_data(file, player_id, &record);

			//Increment the ties variable
			record.ties++;

			printf("Saving record .... \n");
			print_player_record(&record);

			//Save it back to the database
			index_update(file, player_id, &record);
		}
	}
}

void save_winner(gameserver_t *gameserver, indexed_file_t *file, player_t winner)
{
	int i;
	for (i = 0; i < gameserver->player_size; i++)
	{
		int player_id = gameserver->players[i].player_id;

		//See if the current player is a winner
		if (gameserver->players[i].player == winner)
		{
			//See if the index exists if it doesn't create it
			if (index_get_index(file, player_id) == -1)
			{
				//Create the record
				player_record_t record;

				//TODO have user tell us who he is
				player_rec_init(&record, player_id, "Joe", "Smith");

				//Set the number of ties to 1
				record.wins = 1;

				printf("Adding record .... \n");
				print_player_record(&record);

				//Add the record to the database
				index_add(file, player_id, &record);
			}
			else
			{
				//Create a buffer
				player_record_t record;

				//Load the record from the database to the buffer
				index_get_data(file, player_id, &record);

				//Increment the ties variable
				record.wins++;

				printf("Saving record .... \n");
				print_player_record(&record);

				//Save it back to the database
				index_update(file, player_id, &record);
			}
		}
		else
		{
			//See if the index exists if it doesn't create it
			if (index_get_index(file, player_id) == -1)
			{
				//Create the record
				player_record_t record;

				//TODO have user tell us who he is
				player_rec_init(&record, player_id, "Joe", "Smith");

				//Set the number of ties to 1
				record.losses = 1;

				printf("Adding record .... \n");
				print_player_record(&record);

				//Add the record to the database
				index_add(file, player_id, &record);
			}
			else
			{
				//Create a buffer
				player_record_t record;

				//Load the record from the database to the buffer
				index_get_data(file, player_id, &record);

				//Increment the ties variable
				record.losses++;

				printf("Saving record .... \n");
				print_player_record(&record);

				//Save it back to the database
				index_update(file, player_id, &record);
			}
		}
	}
}

void update_stats(gameserver_t *gameserver, indexed_file_t *file)
{
	//Get the winner
	player_t winner = ttt_winner(gameserver->game);

	//See if the game ended in a draw
	if (winner == NEITHER)
	{
		save_draw(gameserver, file);
	}
	else
	{
		save_winner(gameserver, file, winner);
	}
}

int main()
{
//Register signal callback
signal(SIGCHLD, chld_signal_handler);
signal(SIGINT, int_signal_handler);

if (fork() == 0)
{
	server_t *chatserver = (server_t *) malloc(sizeof(server_t));
	server_init(chatserver, "192.168.1.3", 32601, 10);
	server_start(chatserver);

	while (1)
	{
		server_handle(chatserver);

		if (chatserver->client_size == 2)
		{
			subserver_t *subserver = (subserver_t *) malloc(sizeof(subserver_t));

			subserver_init(subserver, chatserver->clients, chatserver->client_size);

			if (fork() == 0)
			{
				chatserver_t *chatserver = (chatserver_t *) malloc(sizeof(chatserver_t));

				chatserver_init(chatserver, subserver);

				while (1)
				{
					chatserver_handle(chatserver);
				}
			}
			else
			{
				int i;
				for (i = 0; i < gameserver->client_size; i++)
				{
					if (close(gameserver->clients[i]) == -1)
					{
						perror("Closing failed");
					}
				}

				chatserver->client_size = 0;
			}
		}
	}
}
else
{
	//Declare our database
	database = (indexed_file_t *) malloc(sizeof(indexed_file_t));
	index_init(database, "stats.master", "stats.index", sizeof(player_record_t));

	//Init and start the listening server
	gameserver = (server_t *) malloc(sizeof(server_t));
	server_init(gameserver, "192.168.1.3", 32600, 10);
	server_start(gameserver);

	//Init our semaphore
	semaphore_t *mutex = (semaphore_t *) malloc(sizeof(semaphore_t));
	if (semaphore_init(mutex, 1, ftok("semaphore.txt", 'b')) == -1)
	{
		printf("Error can not init semaphore\n");
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		server_handle(gameserver);

		if (gameserver->client_size == 2)
		{
			subserver_t *subserver = (subserver_t *) malloc(sizeof(subserver_t));

			subserver_init(subserver, gameserver->clients, gameserver->client_size);

			if (fork() == 0)
			{
				printf("Creating game server\n");

				gameserver_t *gameserver = (gameserver_t *) malloc(sizeof(gameserver_t));

				gameserver_init(gameserver, subserver);

				while (!gameserver_is_finished(gameserver))
				{
					gameserver_handle(gameserver);
				}

				printf("Game is over saving stats\n");

				//Make sure that we can enter
				if (semaphore_wait(mutex) == -1)
				{
					exit(EXIT_FAILURE);
				}

				//Open a database transaction
				index_open_transaction(database);

				//Save the statics
				update_stats(gameserver, database);

				//Close a database transaction
				index_close_transaction(database);

				//Tell other processes that we are done
				if (semaphore_signal(mutex) == -1)
				{
					exit(EXIT_FAILURE);
				}

				printf("Gameserver is shutting down\n");

				gameserver_close(gameserver);

				free(gameserver);

				exit(EXIT_SUCCESS);
			}
			else
			{
				int i;
				for (i = 0; i < gameserver->client_size; i++)
				{
					if (close(gameserver->clients[i]) == -1)
					{
						perror("Closing failed");
					}
				}

				gameserver->client_size = 0;
			}
		}
	}
}

return 0;
}
