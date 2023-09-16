#include <fcntl.h>
#include "posix.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>

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

  sem1 = sem_open(SEMNAME1, O_CREAT, 0600, 1);
  if (sem1 == SEM_FAILED)
    err_exit("sem_open");
  sem2 = sem_open(SEMNAME2, O_CREAT, 0600, 0);
  if (sem2 == SEM_FAILED)
    err_exit("sem_open");

  fd = shm_open(SHMNAME, O_CREAT | O_RDWR, 0600);
  if (fd == -1)
    err_exit("shm_open");

  if (ftruncate(fd, SIZE) == -1)
    err_exit("ftruncate");

  shmbuf = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shmbuf == (void *) -1)
    err_exit("mmap");
  
  if (sem_wait(sem1) == -1)
    err_exit("sem_wait");
  snprintf(shmbuf, SIZE, "%s\n", "Hello!");
  printf("Server send: %s", shmbuf);
  if (sem_post(sem2) == -1)
    err_exit("sem_post");
  
  if (sem_wait(sem1) == -1)
    err_exit("sem_wait");
  printf("Server receive: %s", shmbuf);

  if (munmap(shmbuf, SIZE) == -1)
    err_exit("munmap");

  if (sem_close(sem1) == -1)
    err_exit("sem_close");
  if (sem_close(sem2) == -1)
    err_exit("sem_close");

  if (sem_unlink(SEMNAME1) == -1)
    err_exit("sem_unlink");
  if (sem_unlink(SEMNAME2) == -1)
    err_exit("sem_unlink");

  if (shm_unlink(SHMNAME) == -1)
    err_exit("shm_unlink");
return EXIT_SUCCESS;
}
