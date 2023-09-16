#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define err_exit(msg)   do { perror(msg); exit(EXIT_FAILURE); } while(0)

int main(void) {
  sigset_t sigmask;
  int ret;
  int sig; /* Catched signal */
  /* Blocking signal SIGUSR1 */
  if (sigemptyset(&sigmask) == -1)
    err_exit("sigemptyset");
  if (sigaddset(&sigmask, SIGUSR1) == -1)
    err_exit("sigaddset");
  if (sigprocmask(SIG_BLOCK, &sigmask, NULL) == -1)
    err_exit("sigprocmask");
  
  printf("pid: %d\n", getpid());
  printf("Infinite loop: must be interrupted\n");

  /* Waiting SIGUSR1 and then do something */
  while((ret = sigwait(&sigmask, &sig)) == 0)
    printf("Event (the SIGUSR1 signal is caught)\n");

  if (ret != 0)
    err_exit("sigwait");
  return EXIT_SUCCESS;
}
