/*
 * semaphore.h
 *
 *  Created on: Nov 26, 2013
 *      Author: stefan
 */

#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#define MAX_RETRIES 10

typedef struct
{
	int id;
} semaphore_t;

int semaphore_init(semaphore_t *semaphore, int value, int key);
void semaphore_remove(semaphore_t *semaphore);
void semaphore_wait(semaphore_t *semaphore);
void semaphore_signal(semaphore_t *semaphore);

#endif /* SEMAPHORE_H_ */
