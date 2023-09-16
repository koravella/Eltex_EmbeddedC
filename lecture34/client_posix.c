#include <fcntl.h>
#include "posix.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <errno.h>

#define err_exit(msg)   do { perror(msg); exit(EXIT_FAILURE); } while(0)

int main(void) {
  int status;
  int fd;
  char *shmbuf;
  sem_t *sem1, *sem2;
/*
   * sem1 == 1 -> Server work with shmbuf,
   * sem2 == 1 -> Client work with smbuf
   */
  
  sem1 = sem_open(SEMNAME1, 0);
  if (sem1 == SEM_FAILED)
    err_exit("sem_open");
  sem2 = sem_open(SEMNAME2, 0);
  if (sem2 == SEM_FAILED)
    err_exit("sem_open");

  fd = shm_open(SHMNAME, O_RDWR, 0);
  if (fd == -1)
    err_exit("shm_open");

  shmbuf = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shmbuf == (void *) -1)
    err_exit("mmap");
  
  if (sem_wait(sem2) == -1)
    err_exit("sem_wait");
  printf("Client receive: %s", shmbuf);
  snprintf(shmbuf, SIZE, "%s\n", "Hi!");
  printf("Client send: %s", shmbuf);
  if (sem_post(sem1) == -1)
    err_exit("sem_post");
 
  if (munmap(shmbuf, SIZE) == -1)
    err_exit("munmap");

  if (sem_close(sem1) == -1)
    err_exit("sem_close");
  if (sem_close(sem2) == -1)
    err_exit("sem_close");
return EXIT_SUCCESS;
}
