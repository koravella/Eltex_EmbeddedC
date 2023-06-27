#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
  int status;

  if (fork() == 0) {
    if (fork() == 0)
      ;
    else
      wait(&status);
  }
  else {
    if (fork() == 0) {
      if (fork() == 0)
        ;
      else {
        if (fork() == 0)
          ;
        else {
          wait(&status);
          wait(&status);
        }
      }
    }
    else {
      wait(&status);
      wait(&status);
    }
  }
  printf("pid = %d, ppid = %d\n", getpid(), getppid());
  return 0;
}
