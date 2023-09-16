#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "systemv.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define err_exit(msg)   do { perror(msg); exit(EXIT_FAILURE); } while(0)

int main(void) {
  key_t key1, key2;
  int shmid;
  int semid;
  int status;
  union semun arg;
  char *str;

  key1 = ftok(BINNAME, 0);
  if (key1 == -1)
    err_exit("ftok");
  key2 = ftok(BINNAME, 1);
  if (key2 == -1)
    err_exit("ftok");
  
  semid = semget(key2, 2, IPC_CREAT | 0600);
  if (semid == -1)
    err_exit("semget");

  arg.val = 1;
  status = semctl(semid, 0, SETVAL, arg);
  if (status == -1)
    err_exit("semctl");
  arg.val = 0;
  status = semctl(semid, 1, SETVAL, arg);
  if (status == -1)
    err_exit("semctl");

  shmid = shmget(key1, SIZE, IPC_CREAT | 0600);
  if (shmid == -1)
    err_exit("shmget");
  
  str = (char*) shmat(shmid, NULL, 0);
  if (str == (void *) -1)
    err_exit("shmat");

  semop(semid, &serv_lock, 1);
  snprintf(str, SIZE, "%s\n", "Hello!");
  printf("Server send: %s", str);
  semop(semid, &serv_unlock, 1);

  semop(semid, &serv_lock, 1);
  printf("Server receive: %s", str);
  semop(semid, &serv_unlock, 1);

  status = shmdt(str);
  if (status == -1)
    err_exit("shmdt");

  status = shmctl(shmid, IPC_RMID, NULL);
  if (status == -1)
    err_exit("shmctl");
  status = semctl(semid, 0, IPC_RMID);
  if (status == -1)
    err_exit("semctl");

  return EXIT_SUCCESS;
}
