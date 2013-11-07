#include "game.h"

void ttt_init_game(game_t *game, int rows, int columns, int connect, cell_t **grid)
{
	//Init our game struct
	game->grid = grid;
	game->rows = rows;
	game->columns = columns;
	game->connect = connect;
	game->turn = PLAYER1;
	game->turns = 0;

	//Go thru all the cells and set occupied to false
	int i, j;
	for (i = 0; i < game->rows; i++)
	{
		for (j = 0; j < game->columns; j++)
		{
			cell_t *cell = ttt_get_cell(game, i, j);
			cell->player = NEITHER;
			cell->occupied = 0;
		}
	}
}

void ttt_turnover_turn(game_t *game)
{
	if (game->turn == PLAYER1)
	{
		game->turn = PLAYER2;
	}
	else
	{
		game->turn = PLAYER1;
	}
}

int ttt_make_move(game_t *game, int row, int column)
{
	//Make sure that a player hasn't gone their yet
	if (ttt_is_pos_occupied(game, row, column))
	{
		return 0;
	}

	//Get the cell at the location
	cell_t *cell = ttt_get_cell(game, row, column);

	//Make sure that the cell was inbounds
	if (cell == 0)
	{
		return 0;
	}

	//Set the cell to occupied and insert the player
	cell->occupied = 1;
	cell->player = game->turn;

	//Turnover the turn
	ttt_turnover_turn(game);
	game->turns++;

	return 1;
}

cell_t *ttt_get_cell(game_t *game, int row, int column)
{
	//Make sure that we are in bounds
	if (!ttt_is_in_bounds(game, row, column))
	{
		return 0;
	}

	return &(game->grid[row][column]);
}

player_t ttt_winner(game_t *game)
{
	//TODO support more than 3x3 boards
	int i = 0;
	int j = 0;

	for (i = 0; i < game->rows; i++)
	{
		for (j = 0; j < game->columns; j++)
		{
			player_t player = ttt_check_winner_cell(game, i, j);

			if (player != NEITHER)
			{
				return player;
			}
		}
	}

	return NEITHER;
}

player_t ttt_check_array(player_t *array, int length)
{
	int i = 0;
	player_t player = array[0];

	for (i = 1; i < length; i++)
	{
		if (player != array[i])
		{
			return NEITHER;
		}
	}

	return player;
}

player_t ttt_check_winner_cell(game_t *game, int row, int column)
{
	player_t player = NEITHER;

	//Check vertical
	player = ttt_check_vertical(game, row, column);

	if (player != NEITHER)
	{
		return player;
	}

	//Check horizontal
	player = ttt_check_horizontal(game, row, column);

	if (player != NEITHER)
	{
		return player;
	}

	//Check diagonal north west
	player = ttt_check_diagonal_nw(game, row, column);

	if (player != NEITHER)
	{
		return player;
	}

	//Check diagonal south east
	player = ttt_check_diagonal_se(game, row, column);

	if (player != NEITHER)
	{
		return player;
	}

	return NEITHER;
}

player_t ttt_check_vertical(game_t *game, int row, int column)
{
	int i = 0;
	player_t cache[game->connect];

	//Check vertical
	for (i = 0; i < game->connect; i++)
	{
		if (ttt_is_in_bounds(game, row + i, column))
		{
			cache[i] = game->grid[row + i][column].player;
		}
		else
		{
			return NEITHER;
		}
	}

	return ttt_check_array(cache, game->connect);
}

player_t ttt_check_horizontal(game_t *game, int row, int column)
{
	int i = 0;
	player_t cache[game->connect];

	for (i = 0; i < game->connect; i++)
	{
		if (ttt_is_in_bounds(game, row, column + i))
		{
			cache[i] = game->grid[row][column + i].player;
		}
		else
		{
			return NEITHER;
		}
	}

	return ttt_check_array(cache, game->connect);
}

player_t ttt_check_diagonal_se(game_t *game, int row, int column)
{
	int i = 0;
	player_t cache[game->connect];

	for (i = 0; i < game->connect; i++)
	{
		if (ttt_is_in_bounds(game, row + i, column + i))
		{
			cache[i] = game->grid[row + i][column + i].player;
		}
		else
		{
			return NEITHER;
		}
	}

	return ttt_check_array(cache, game->connect);
}

player_t ttt_check_diagonal_nw(game_t *game, int row, int column)
{
	int i = 0;
	player_t cache[game->connect];

	for (i = 0; i < game->connect; i++)
	{
		if (ttt_is_in_bounds(game, row + i, column - i))
		{
			cache[i] = game->grid[row + i][column - i].player;
		}
		else
		{
			return NEITHER;
		}
	}

	return ttt_check_array(cache, game->connect);
}
int ttt_is_in_bounds(game_t *game, int row, int column)
{
	return !((row < 0) || (column < 0) || (row >= game->rows) || (column >= game->columns));
}

int ttt_is_pos_occupied(game_t *game, int row, int column)
{
	return ttt_get_cell(game, row, column)->occupied == 1;
}

int ttt_is_end_of_game(game_t *game)
{
	return ttt_has_winner(game) || ttt_is_grid_filled(game);
}

int ttt_is_grid_filled(game_t *game)
{
	return (game->turns == (game->rows * game->columns));
}

int ttt_has_winner(game_t *game)
{
	return ttt_winner(game) != NEITHER;
}
