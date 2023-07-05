#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define err_exit(msg)   do {perror(msg); exit(EXIT_FAILURE);} while (0)

void* print_num(void *arg) {
  int *num = arg;
  printf("Thread %d is being executed\n", *num);
  return NULL;
}

int main(int argc, char* argv[]) {
  int sum, num_threads, st;
  
  if (argc != 2) {
    fprintf(stderr, "Usage: %s num-threads\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  num_threads = atoi(argv[1]);
  if (num_threads < 0) {
    fprintf(stderr, "Wrong num-threads\n");
    exit(EXIT_FAILURE);
  }

  pthread_t *threads = malloc(num_threads * (sizeof (pthread_t)));
  if (threads == NULL)
    err_exit("malloc");

  int *nums = malloc(num_threads * (sizeof (int)));
  if (nums == NULL)
    err_exit("malloc");
  
  for (int i = 0; i < num_threads; i++) {
    nums[i] = i;
    st = pthread_create(&threads[i], NULL, print_num, &nums[i]);
    if (st != 0)
      err_exit("pthread_create");
  }

  for (int i = 0; i < num_threads; i++) {
    st = pthread_join(threads[i], NULL);
    if (st != 0)
      err_exit("pthread_create");
  }

  free(threads);
  free(nums);
  return 0;
}
