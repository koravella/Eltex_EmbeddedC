#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#define errExit(msg)    do {perror(msg); exit(EXIT_FAILURE);} while (0)
#define TIMEFACTOR 10
#define CPU 0

int main(int argc, char *argv[]) {
  cpu_set_t set;
  long s = 0, max;
  int nc;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s inc-nice\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  
  CPU_SET(CPU, &set);
  max = (long) INT_MAX*TIMEFACTOR;
  nc = atoi(argv[1]);

  if (sched_setaffinity(getpid(), sizeof(set), &set) == -1)
    errExit("sched_setaffinity");

  if (fork() == 0) {
    while (s != max)
      ++s;
    printf("child finished\n");
    }
  else {
    if (nice(nc) == -1)
      errExit("nice");
    while (s != max)
      ++s;
    printf("parent finished\n");
    wait(NULL);
  }
  return 0;
}
