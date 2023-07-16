#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define err_exit(msg)   do {perror(msg); exit(EXIT_FAILURE);} while (0)

// Number of shops, producers and customers:
#define NSHOPS 5
#define NPROD 1
#define NCUST 3

// All the demand of each customer and one-time supply of each producer:
#define DEMAND 5000
#define SUPPLY 500

//Default value in each shop:
#define DEFSUP 1000

// Sleep time of each producer and customer, s:
#define PSLEEP 2
#define CSLEEP 1

//Size buffer for logs
#define BUFFSIZE 1024

int shops[NSHOPS];
pthread_mutex_t muts[NSHOPS];

void logger(char* str) {
  time_t t = time(NULL);
  struct tm *now = localtime(&t);
  printf("[%02d/%02d/%4d:%02d:%02d:%02d] %s\n", now->tm_mday, now->tm_mon + 1,
         now->tm_year + 1900, now->tm_hour, now->tm_min, now->tm_sec, str);
}

void* producer(void* arg) {
  int *id = arg;
  char str[BUFFSIZE];
  int st;
  while (1) {
    size_t k = rand() % NSHOPS;
    pthread_mutex_lock(&muts[k]);
    shops[k] += SUPPLY;
    st = snprintf(str, BUFFSIZE,
                  "shop: %2ld, producer: %2d, changes: %5d, remains: %5d",
                  k, *id, SUPPLY, shops[k]);
    if (st >= 0)
      logger(str);
    else
      err_exit("snprintf");
    pthread_mutex_unlock(&muts[k]);
    sleep(PSLEEP);
  }
  return NULL;
}

void* customer(void* arg) {
  int *id = arg;
  int rem = DEMAND;
  char str[BUFFSIZE];
  int st;
  while (rem > 0) {
    size_t k = rand() % NSHOPS;
    pthread_mutex_lock(&muts[k]);
    rem -= shops[k];
    st = snprintf(str, BUFFSIZE,
            "shop: %2ld, customer: %2d, changes: %5d, remains: %5d, needs: %5d",
                  k, *id, shops[k], 0, rem);
    if (st >= 0)
      logger(str);
    else
      err_exit("snprintf");
    shops[k] = 0;
    pthread_mutex_unlock(&muts[k]);
    sleep(CSLEEP);
  }
  return NULL;
}

int main(void) {
  int prod_id[NPROD];
  int cust_id[NCUST];
  pthread_t prods[NPROD];
  pthread_t custs[NCUST];
  int st;

  srand(time(NULL));

  for (size_t i = 0; i < NSHOPS; i++) {
    shops[i] = DEFSUP;
    pthread_mutex_init(&muts[i], NULL);
  }

  for (int i = 0; i < NPROD; i++) {
    prod_id[i] = i;
    st = pthread_create(&prods[i], NULL, producer, &prod_id[i]);
    if (st != 0)
      err_exit("pthread_create");
  }
  for (int i = 0; i < NCUST; i++) {
    cust_id[i] = i;
    st = pthread_create(&custs[i], NULL, customer, &cust_id[i]);
    if (st != 0)
      err_exit("pthread_create");
  }

  for (int i = 0; i < NCUST; i++) {
    st = pthread_join(custs[i], NULL);
    if (st != 0)
      err_exit("pthread_join");
  }
  for (int i = 0; i < NPROD; i++) {
    st = pthread_cancel(prods[i]);
    if (st != 0)
      err_exit("pthread_cancel");
  }
  return 0;
}
