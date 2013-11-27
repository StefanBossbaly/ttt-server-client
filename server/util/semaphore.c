#include "semaphore.h"

#ifdef SEMAPHORE_DEBUG
#include <stdio.h>
#endif

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

void semaphore_init(semaphore_t *semaphore, int value, int key)
{
	  //See if there is already a semaphore with the value of key
	  if ((semaphore->id = semget(key, 1, 0)) == -1)
	  {
		  if (errno == ENOENT)
		  {
			  semaphore->id = semget(key, 1,  0777 | IPC_CREAT);
			  union semun arg;
			  arg.val = value;
			  semctl(semaphore->id, 0, SETVAL, arg);

#ifdef SEMAPHORE_DEBUG
			  printf("Semaphore created! id = %i, value %i\n", id, value);
#endif
		  }
		  else
		  {
			  perror("semaphore_init");
		  }
	  }
}

void semaphore_remove(semaphore_t *semaphore)
{
    union semun arg;
    semctl(semaphore->id, 0, IPC_RMID, arg);
}

void semaphore_wait(semaphore_t *semaphore)
{
	  struct sembuf	p_buf;
	  p_buf.sem_num = 0;
	  p_buf.sem_op = -1;	     // subtract 1 indicates P
	  p_buf.sem_flg = 0;

	  semop(semaphore->id, &p_buf, 1);
}

void semaphore_signal(semaphore_t *semaphore)
{
	  struct sembuf	v_buf;
	  v_buf.sem_num = 0;
	  v_buf.sem_op = 1;          // add 1 indicates V
	  v_buf.sem_flg = 0;

	  semop(semaphore->id, &v_buf, 1);
}
