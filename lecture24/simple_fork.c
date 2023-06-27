#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void term(void) {
  printf("from terminate %d\n", getpid());
}

int main(void) {
  pid_t child_pid;
  int status;

  atexit(term);
  child_pid = fork();
  if (child_pid == 0) {
    printf("from child %d\n", getpid());
    execl("/bin/ps", "ps", "r", NULL);
    exit(EXIT_FAILURE);
  }
  else {
    printf("from parent %d\n", getpid());
    wait(&status);
  } 
  return 0;
}
