#include "player_record.h"
#include <string.h>

void player_rec_init(player_record_t *record, int id, char *first_name, char *last_name)
{
	record->player_id = id;

	strcpy(record->first_name, first_name);
	strcpy(record->last_name, last_name);

	record->wins = 0;
	record->losses = 0;
	record->ties = 0;
}
