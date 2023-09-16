/*
 * Creates and deletes a shared memory for messages and a semaphore
 * to access it.
 */

#include "declarations.h"
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <errno.h>

/* Macros for early termination */
#define err_exit(msg)   do { perror(msg); exit(EXIT_FAILURE); } while(0)

int main(void) {
  static struct termios oldt, newt;
  int status;
  int fd;
  struct Bufmsg *shmbuf;
  sem_t *sem_access;     /* 1 -- enable, 0 -- disable: access to the 'shmbuf' */

  /* Creating shared memory segment */ 
  fd = shm_open(SHMNAME, O_CREAT | O_EXCL | O_RDWR, 0600);
  if (fd == -1 && errno == EEXIST) {
    if (shm_unlink(SHMNAME) == -1)
      err_exit("shm_unlink");
    fd = shm_open(SHMNAME, O_CREAT | O_EXCL | O_RDWR, 0600);
  }
  if (fd == -1)
    err_exit("shm_open");
  if (ftruncate(fd, sizeof(struct Bufmsg)) == -1)
    err_exit("ftruncate");
  shmbuf = mmap(NULL, sizeof(struct Bufmsg),
                PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shmbuf == (void *) -1)
    err_exit("mmap");
  
  /* Creating and initializing a named semaphore for access to shared memory */ 
  sem_access = sem_open(SEMNAME, O_CREAT | O_EXCL, 0600, 1);
  if (sem_access == SEM_FAILED && errno == EEXIST) {
    if (sem_unlink(SEMNAME) == -1)
      err_exit("sem_unlink");
    sem_access = sem_open(SEMNAME, O_CREAT | O_EXCL, 0600, 1);
  }
  if (sem_access == SEM_FAILED)
    err_exit("sem_open");

  /* Waiting for the finalisation command */
  printf("Enter 'q' to stop server and quit\n");
  tcgetattr( STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr( STDIN_FILENO, TCSANOW, &newt);
  while (getchar() != 'q');
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt);

  /* Stopping the server */
  if (sem_wait(sem_access) == -1)
    err_exit("sem_wait");
  if (shmbuf->num_msgs < MAX_MESSAGES) {
    shmbuf->msgs[shmbuf->num_msgs].type = STOP;
    shmbuf->num_msgs++;
  }
  if (sem_post(sem_access) == -1)
    err_exit("sem_post");

  /* Closing and deleting shared memory and semaphore */
  if (munmap(shmbuf, sizeof(struct Bufmsg)) == -1)
    err_exit("munmap");
  if (sem_close(sem_access) == -1)
    err_exit("sem_close");
  if (sem_unlink(SEMNAME) == -1)
    err_exit("sem_unlink");
  if (shm_unlink(SHMNAME) == -1)
    err_exit("shm_unlink");
  return EXIT_SUCCESS;
}
