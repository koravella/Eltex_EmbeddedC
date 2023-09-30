/*
 * A server that creates a thread pool in advance for processing clients and
 * assigns each new client to a specific thread.
 */

#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <semaphore.h>

/* Macros for termination */
#define err_exit(msg)   do { perror(msg); exit(EXIT_FAILURE); } while(0)
#define err_exit2(msg)  do { fprintf(stderr, msg); \
                             exit(EXIT_FAILURE); } \
                             while(0)

#define PORT 8080             /* Port of server */
#define ADDR INADDR_LOOPBACK  /* Addres of server */
#define LNUM 5                /* Lenght of queue of clients for listen() */
#define NUM_SERVING 10        /* Number of serving threads */

int thread_free[NUM_SERVING]; /* Indicating free threads: 1 - free, 0 - busy */
pthread_mutex_t mut;          /* Mutex on access to thread_free[] */

sem_t sem_free_threads;         /* Contain number of free threads */
int clients[NUM_SERVING];       /* Contain clients descriptors for threads */
sem_t sem_thread[NUM_SERVING];  /* Used when assigning a client to a thread:
                                   0 - no client, 1 - else */

/*
 * The function of working with a specific client.
 * Accepts a descriptor of client and sends the current time.
 */
void* handler(void *arg) {
  time_t cur_time;
  int id;           /* Number for self-identification of thread */
  int cfd;
  
  id = *((int *) arg);
  
  /* Waiting for the client */
  while (sem_wait(&sem_thread[id]) != -1) {
    cfd = clients[id];

    /* Processing client */
    cur_time = time(NULL);
    if (send(cfd, &cur_time, sizeof(cur_time), 0) == -1) {
      perror("send");
      pthread_exit(0);
    }
    if (close(cfd) == -1) {
      perror("close");
      pthread_exit(0);
    }
    
    /* Notification of completion of client processing */
    if (pthread_mutex_lock(&mut) != 0)
      err_exit2("Error: pthread_mutex_lock\n");
    thread_free[id] = 1;
    if (pthread_mutex_unlock(&mut) != 0)
      err_exit2("Error: pthread_mutex_unlock\n");
    
    if (sem_post(&sem_free_threads) == -1)
      err_exit("sem_post");
  }
  err_exit("sem_wait");
}

int main(void) {
  struct sockaddr_in server;
  struct sockaddr_in client;
  int client_size;
  int lfd;                    /* Listening socket */
  int cfd;                    /* Client socket */
  int *copy_cfd;
  pthread_attr_t attr;
  pthread_t threads[NUM_SERVING];
  int st;
  int k;
  int opt;                    /* bool option for setsockopt() */
  int thread_id[NUM_SERVING]; /* Numbers for self-identification of threads */

  /* Creating service threads */
  for (int i = 0; i < NUM_SERVING; i++)
    thread_free[i] = 1;
  if (pthread_mutex_init(&mut, NULL) != 0)
    err_exit2("Error: pthread_mutex_init\n");

  for (int i = 0; i < NUM_SERVING; i++) {
    if (sem_init(&sem_thread[i], 0, 0) == -1)
      err_exit("sem_init");
    thread_id[i] = i;
    if (pthread_create(&threads[i], NULL, handler, &thread_id[i]) != 0)
      err_exit2("Error: pthread_create\n");
  }
  if (sem_init(&sem_free_threads, 0, NUM_SERVING) == -1)
    err_exit("sem_init");

  /* Creating a listening socket */
  lfd = socket(AF_INET, SOCK_STREAM, 0);
  if (lfd == -1)
    err_exit("socket");
  
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = htonl(ADDR);

  opt = 1;
  if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) == -1)
    err_exit("setsockopt");
  
  if (bind(lfd, (struct sockaddr*) &server, sizeof(server)) == -1)
    err_exit("bind");

  if (listen(lfd, LNUM) == -1)
    err_exit("listen");

  /* Processing clients: infinity loop */ 
  client_size = sizeof(struct sockaddr_in);
  while ((cfd = accept(lfd, (struct sockaddr *) &client, &client_size)) != -1) {
    /* Search for a free thread */ 
    if (sem_wait(&sem_free_threads) == -1)
      err_exit("sem_wait");
    
    if (pthread_mutex_lock(&mut) != 0)
      err_exit2("Error: pthread_mutex_lock\n");
    for (k = 0; k < NUM_SERVING; k++)
      if (thread_free[k] == 1) {
        thread_free[k] = 0;
        break;
      }
    if (pthread_mutex_unlock(&mut) != 0)
      err_exit2("Error: pthread_mutex_unlock\n");

    /* Passing the next client to the found thread */
    clients[k] = cfd;
    if (sem_post(&sem_thread[k]) == -1)
      err_exit("sem_post");
    client_size = sizeof(struct sockaddr_in);
  }
  err_exit("accept");
}
