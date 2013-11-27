/*
 * player_record.h
 *
 *  Created on: Nov 26, 2013
 *      Author: stefan
 */

#ifndef PLAYER_RECORD_H_
#define PLAYER_RECORD_H_

typedef struct {
   int player_id;
   char first_name[10];
   char last_name[10];
   int wins;
   int losses;
   int ties;
} player_record_t;

void player_rec_init(player_record_t *record, int id, char *first_name, char *last_name);

#endif /* PLAYER_RECORD_H_ */
