#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX 5

int main(void) {
  while (1) {
    char cmd[MAX];
    if (fgets(cmd, MAX, stdin)) {
      ;
    }
    else {
      perror("Ошибка считывания команды");
    }
    int c;
    while((c = getchar()) != '\n' && c != EOF);
  }
}
