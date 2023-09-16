#ifndef SYSTEMV_H
#define SYSTEMV_H

#include <sys/types.h>
#include <sys/sem.h>

#define BINNAME "server_systemv.out"  /* Pathname for ftok */
#define SIZE 128                      /* Size of shared memory segment */

/* structure for SystemV semaphor */
union semun {
  int              val;    /* Value for SETVAL */
  struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
  unsigned short  *array;  /* Array for GETALL, SETALL */
  struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

/* 
 * semaphors (x,y): x = 1 -- server may write, x = 0 -- server wait 
 *                  y = 1 -- client may write, y = 0 -- client wait
 */
struct sembuf serv_lock = {0, -1, 0};
struct sembuf serv_unlock = {1, 1, 0};
struct sembuf clie_lock = {1, -1, 0};
struct sembuf clie_unlock = {0, 1, 0};

#endif
