#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define err_exit(msg)   do {perror(msg); exit(EXIT_FAILURE);} while (0)

int main(void) {
  int pc_fd[2], cp_fd[2];
  char buf;
  char msg1[] = "Hello!\n", msg2[] = "Hi!\n";
  int cpid;
  int st;

  if (pipe(pc_fd) == -1 || pipe(cp_fd) == -1)
    err_exit("pipe");

  if ((cpid = fork()) == -1)
    err_exit("fork");

  if (cpid == 0) {
    close(pc_fd[1]);
    close(cp_fd[0]);
    
    printf("Child read: ");
    while ((st = read(pc_fd[0], &buf, 1)) != 0) {
      if (st == -1)
        err_exit("read");
      putchar(buf);
    }
    close(pc_fd[0]);
    
    printf("Child write %s", msg2);
    if (write(cp_fd[1], msg2, strlen(msg2)) == -1)
      err_exit("write");
    close(cp_fd[1]);
  }
  else {
    close(pc_fd[0]);
    close(cp_fd[1]);
    
    printf("Parent write %s", msg1);
    if (write(pc_fd[1], msg1, strlen(msg1)) == -1)
      err_exit("write");
    close(pc_fd[1]);

    printf("Parent read: ");
    while ((st = read(cp_fd[0], &buf, 1)) != 0) {
      if (st == -1)
        err_exit("read");
      putchar(buf);
    }
    close(cp_fd[0]);
    
    wait(NULL);
  }
  return EXIT_SUCCESS;
}


