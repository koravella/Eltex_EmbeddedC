#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>

#define MAXLINE 1024    /* Maximum lenght of input line with commands */
#define MAXARGS 32      /* Maximum number of arguments for each command */
#define MAXPIPES 16     /* Maximum number of pipes between commands */

#define err_exit(msg)   do {perror(msg); exit(EXIT_FAILURE);} while (0)

#define NOCMD 2
#define PIPE 1
#define NOPIPE 0

#define NOEXIT -2

#define NORMAL 0
#define FORCE_EXIT 1

void Promt(void);                       /* Printing a promt */
void GetLineCmds(char *line, int lim);  /* Get input line with commands */

/* 
 * Check exit command. Return -1 on error, exit code on exit an NOEXIT
 * otherwise. Last arg in args must be NULL.
 */
int CheckExit(char *args[], int nargs);

/* 
 * Extracts the following command from the line of commands. Argument line must
 * be set NULL on the second and subsequent calls. Return -1 if an error occurs,
 * NOCMD if the line has ended, NOPIPE if the next command ends with pipe and
 * PIPE otherwise. If the next command is not empty, then the last argument will
 * be NULL. If the next command is empty, then nargs will be 0.
 */
int GetNextCmd(char *line, char *sep, char **path, char *args[], int *nargs);

int main(void) {
  char cmd[MAXLINE];
  char *args[MAXARGS];
  char *path;
  int nargs;
  pid_t cpid;
  int pipefd[MAXPIPES][2];
  int before, after;
  int exit_status, parse_status;
  int cnt;
  char *line;

  while (1) {
    exit_status = EXIT_SUCCESS;
    parse_status = NORMAL;

    Promt();
    GetLineCmds(cmd, MAXLINE);
    
    before = NOPIPE;
    nargs = 0;
    cnt = 0;
    line = cmd;
    while ((after = GetNextCmd(line, " \n", &path, args, &nargs)) != NOCMD) {
      /* Checking the allowed number of pipes */
      if (cnt == MAXPIPES && after == PIPE) {
        fprintf(stderr, "Error: too many pipes");
        if (before == PIPE)
          close(pipefd[cnt - 1][0]);
        break;
      }

      /* Checking for an empty command with pipe */
      if (nargs == 0 && after == PIPE) {
        fprintf(stderr, "Error syntax: empty command with pipe\n");
        break;
      }
      
      /* Processing an exit command */
      exit_status = CheckExit(args, nargs);
      if (exit_status != NOEXIT && before == NOPIPE && after == NOPIPE) {
        parse_status = FORCE_EXIT;
        break;
      }
      else if (exit_status != NOEXIT || exit_status == -1) {
        before = after;
        nargs = 0;
        cnt++;
        line = NULL;
        continue;
      }

      /* Creating a child process to process the command */
      if (after == PIPE && pipe((int *) pipefd + 2 * cnt) == -1)
          err_exit("pipe");
      if ((cpid = fork()) == -1)
        err_exit("fork");
      if (cpid == 0) {
        if (after == PIPE)
          dup2(pipefd[cnt][1], 1);
        if (before == PIPE)
          dup2(pipefd[cnt - 1][0], 0);
        if(execvp(path, args) == -1)
          err_exit("Error in calling the specified program");
      }
      else {
        wait(NULL);
        if (after == PIPE)
          close(pipefd[cnt][1]);
        if (before == PIPE)
          close(pipefd[cnt - 1][0]);
      }

      before = after;
      nargs = 0;
      cnt++;
      line = NULL;
    }
    if (parse_status == FORCE_EXIT)
      break;

    /* PIPE cannot be at the very end of the line of commands */
    if (before == PIPE) {
      fprintf(stderr, "Error syntax: a last command is missing\n");
      close(pipefd[cnt-1][0]);
    }
  }
  return exit_status;
}

void Promt(void) {
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL)
    err_exit("Error in determining the current work directory");
  printf("%s$ ", cwd);
}

void GetLineCmds(char *line, int lim) {
  if (fgets(line, lim, stdin) == NULL)
    err_exit("Error in reading the command");
}

int CheckExit(char *args[], int nargs) {
  if (strcmp(args[0], "exit") != 0)
    return NOEXIT;
  if (nargs > 3) {
    fprintf(stderr, "Error: too many arguments for command exit\n");
    return -1;
  }
  if (nargs == 3)
    return atoi(args[1]);
  return EXIT_SUCCESS;
}

int GetNextCmd(char *line, char *sep, char **path, char *args[], int *nargs) {  
  char *token;
  int ret; // Is there a pipe at the end?
  
  ret = NOPIPE;
  if ((token = strtok(line, sep)) == NULL)
    return NOCMD;
  if (strcmp(token, "|") == 0)
      return PIPE;
  *path = token;
  args[0] = basename(*path);
  for (*nargs = 1; (token = strtok(NULL, sep)) != NULL; (*nargs)++) {
    if (strcmp(token, "|") == 0) {
      ret = PIPE;
      break;
    }
    if (*nargs < MAXARGS - 1)
      args[*nargs] = token;
    else {
      fprintf(stderr, "Error: too many arguments\n");
      return -1;
    }
  }
  args[*nargs] = NULL;
  (*nargs)++;
  return ret;
}




  
