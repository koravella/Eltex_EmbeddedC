#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>

#define MAXCMD 1024
#define MAXARGS 32

void promt(void) {
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    perror("Error in determining the current work directory");
    exit(errno);
  }
  printf("%s$ ", cwd);
}

int main(void) {
  char cmd[MAXCMD];
  char *args[MAXARGS];
  while (1) {
    promt();
    if (fgets(cmd, MAXCMD, stdin) == NULL) {
      //perror("Error in reading the command");
      puts("");
      exit(errno);
    }
    char *token = strtok(cmd, " \n");
    if (token == NULL)
      continue;
    char *path = token;
    args[0] = basename(path);
    int nargs = 1, err = 0;
    while(token) {
      if (nargs < MAXARGS) {
        token = strtok(NULL, " \n");
        args[nargs] = token;
      }
      else {
        err = 1;
        printf("Error: too many arguments\n");
        break;
      }
      ++nargs;
    }
    if (err)
      continue;
    if (strcmp(args[0], "exit") == 0) {
      if (nargs > 3) {
        printf("Error: too many arguments for command exit\n");
        continue;
      }
      else {
        if (nargs == 3)
          exit(*args[1]);
        else
          exit(errno);
      }
    }
    else {
      pid_t child_pid = fork();
      if (child_pid == 0) {
        if(execvp(path, args) == -1)
          perror("Error in calling the specified program");
      }
      else {
        int status;
        wait(&status);
      }
    }
  }
}
