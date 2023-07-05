#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define err_exit(msg)    do {perror(msg); exit(EXIT_FAILURE);} while (0)
#define NUM_FOR_THREAD 100000
#define NUM_THREADS 1000

long res = 0;

void* thread_sum(void* arg) {
  for (long i = 0; i < NUM_FOR_THREAD; i++)
    res++;
  return NULL;
}

int main(void) {
  int st;
  pthread_t threads[NUM_THREADS];

  for (int i = 0; i < NUM_THREADS; i++) {
    st = pthread_create(&threads[i], NULL, thread_sum, NULL);
    if (st != 0)
      err_exit("pthread_create");
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    st = pthread_join(threads[i], NULL);
    if (st != 0)
      err_exit("pthread_join");
  }

  printf("%ld\n", res);
  return 0;
}
