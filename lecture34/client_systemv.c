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
  
  semid = semget(key2, 2, 0);
  if (semid == -1)
    err_exit("semget");
  
  shmid = shmget(key1, SIZE, 0);
  if (shmid == -1)
    err_exit("shmget");
  
  str = (char*) shmat(shmid, NULL, 0);
  if (str == (void *) -1)
    err_exit("shmat");

  semop(semid, &clie_lock, 1);
  printf("Server receive: %s", str);
  snprintf(str, SIZE, "%s\n", "Hi!");
  printf("Server send: %s", str);
  semop(semid, &clie_unlock, 1);

  status = shmdt(str);
  if (status == -1)
    err_exit("shmdt");

  return EXIT_SUCCESS;
}
