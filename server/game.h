/**
 * @file game.h
 * @Author Stefan Bossbaly (Sbossb@gmail.com)
 * @date September 8, 2013
 *
 * This structure defines the game type used by this program.
 */

#ifndef GAME_H_
#define GAME_H_

//Player Enum
typedef enum {PLAYER1 = 1, PLAYER2 = 2, NEITHER = 0} player_t;

/**
 * The cell structure.
 */
typedef struct
{
	int occupied;
	player_t player;
} cell_t;

/**
 * The game structure. Represents qualities that a game would have
 */
typedef struct
{
	player_t turn;
	int rows;
	int columns;
	int connect;
	cell_t **grid;
	int turns;
} game_t;


//Init

/**
 * Initializes the game structure to the default values
 */
void ttt_init_game(game_t *game, int rows, int columns, int connect, cell_t **grid);

//Gameplay

/**
 * Turnovers the turn from player to player
 */
void ttt_turnover_turn(game_t *game);

/**
 * Occupies a cell on the grid with the current player's symbol
 */
int ttt_make_move(game_t *game, int row, int column);

/**
 * Gets the cell that exists at the given location
 *
 * @param row the row of the cell
 * @param column the column of the cell
 *
 * @return Returns the cells at the given location
 */
cell_t *ttt_get_cell(game_t *game, int row, int column);


player_t ttt_check_winner_cell(game_t *game, int row, int column);
player_t ttt_check_vertical(game_t *game, int row, int column);
player_t ttt_check_horizontal(game_t *game, int row, int column);
player_t ttt_check_diagonal_se(game_t *game, int row, int column);
player_t ttt_check_diagonal_nw(game_t *game, int row, int column);

/**
 * Returns the winner of the game
 *
 * @return Returns the winner of the game. If there is no winner this function will return NEITHER
 */
player_t ttt_winner(game_t *game);

//Check conditions

/**
 * Checks to see if the row and column are in grid's bounds
 *
 * @param row the row of the cell
 * @param column the column of the cell
 *
 * @return true if the row and column are contained in the grid; false otherwise
 */
int ttt_is_in_bounds(game_t *game, int row, int column);

/**
 * Checks to see if the position is occupied by a player other than NEITHER
 *
 * @param row the row of the cell
 * @param column the column of the cell
 *
 * @return true if the cell at row and column is occupied; false otherwise
 */
int ttt_is_pos_occupied(game_t *game, int row, int column);

/**
 * Checks to see if the game has ended from either a player winning or a draw condition
 *
 * @return true if the game has ended; false otherwise
 */
int ttt_is_end_of_game(game_t *game);

/**
 * Checks to see if the grid is filled
 *
 * @return true if the grid is filled; false otherwise
 */
int ttt_is_grid_filled(game_t *game);

/**
 * Checks to see if the game has a winner
 *
 * @returns true if the game has a winner; false otherwise
 */
int ttt_has_winner(game_t *game);



#endif /* GAME_H_ */
