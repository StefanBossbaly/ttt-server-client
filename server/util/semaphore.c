#include "semaphore.h"

#ifdef SEMAPHORE_DEBUG
#include <stdio.h>
#endif

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

union semun
{
	int val;
	struct semid_ds *buf;
	ushort *array;
};

int semaphore_init(semaphore_t *semaphore, int value, int key)
{
	union semun arg;
	struct semid_ds buf;
	struct sembuf sb;

	int semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);

	//We got to the address first
	if (semid >= 0)
	{
		sb.sem_op = 1;
		sb.sem_flg = 0;
		arg.val = 1;

		if (semop(semid, &sb, 1) == -1)
		{
			perror("semop");
			semctl(semid, 0, IPC_RMID);
			return -1;
		}

		semaphore->id = semid;
	}
	else if (errno == EEXIST)
	{
		int ready = 0;

		//Get the id of the shared memory
		semid = semget(key, 1, 0);

		//Error happened check errno
		if (semid < 0)
			return -1;

		arg.buf = &buf;

		//Wait for the other process to init the semaphore
		int i;
		for (i = 0; i < MAX_RETRIES && !ready; i++)
		{
			//Get statics of the semaphore
			semctl(semid, 0, IPC_STAT, arg);

			//Check to see if the semaphore was created
			if (arg.buf->sem_otime != 0)
			{
				ready = 1;
			}
			else
			{
				sleep(1);
			}
		}

		if (!ready)
		{
			errno = ETIME;
			return -1;
		}

		semaphore->id = semid;
	}

	return -1;
}

void semaphore_remove(semaphore_t *semaphore)
{
	union semun arg;
	semctl(semaphore->id, 0, IPC_RMID, arg);
}

void semaphore_wait(semaphore_t *semaphore)
{
	struct sembuf sb;

	//The one and only semaphore
	sb.sem_num = 0;
	//Subtract one from the semaphore
	sb.sem_op = -1;
	//Just in case we die the kernel will undo our changes
	sb.sem_flg = SEM_UNDO;

	semop(semaphore->id, &sb, 1);
}

void semaphore_signal(semaphore_t *semaphore)
{
	struct sembuf sb;

	//The one and only semaphore
	sb.sem_num = 0;
	//Add one to the semaphore
	sb.sem_op = 1;
	//Just in case we die the kernel will undo our changes
	sb.sem_flg = SEM_UNDO;

	semop(semaphore->id, &sb, 1);
}
