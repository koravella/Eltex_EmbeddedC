#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define err_exit(msg)   do { perror(msg); exit(EXIT_FAILURE); } while(0)

void handler(int signum) {
  printf("The SIGUSR1 signal is caught\n");
}

int main(void) {
  struct sigaction sa;
  
  /* Fill struct sigaction */
  sa.sa_handler = handler;
  if (sigemptyset(&sa.sa_mask) == -1)
    err_exit("sigemptyset");
  sa.sa_flags = 0;

  /* Installing this handler for SIGUSR1 */
  if (sigaction(SIGUSR1, &sa, NULL) == -1)
    err_exit("sigaction");
  
  printf("pid: %d\n", getpid());
  printf("Infinite loop: must be interrupted\n");

  while(1);

  return EXIT_SUCCESS;
}
