/*
 * A server that creates request queue (with client descriptors) and a thread
 * pool in advance for processing clients. Each thread takes the next client
 * from  queue.
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
#define MAX_NUM_CLIENTS 1000  /*Maximum number of clients in the request queue*/

/* A node of single-linked list with 'int' data field */
struct Node {
  int data;
  struct Node *next;
};

/* A queue on single-linked list of limited size */
struct Queue {
  int size, max_size;
  struct Node *front, *end;
};

struct Queue q_clients;           /* Client descriptors for processing */
pthread_mutex_t mut_q_clients;    /* Mutex on access to 'q_clients' */
sem_t sem_num_free;               /* Free space in the request queue */
sem_t sem_num_clients;            /* Number of clients in request queue */

/*
 * Initialization of queue.
 * Return -1 on error and 0 otherwise
 */
int Qinit (struct Queue *queue, int max_size) {
  if (max_size < 0)
    return -1;
  
  queue->max_size = max_size;
  queue->size = 0;
  queue->front = queue->end = NULL;
  return 0;
}

/*
 * Adding 'data' to end of 'queue'.
 * Return -1 on error and 0 otherwise
 */
int Qpush (struct Queue *queue, int data) {
  struct Node *buf;

  if (queue->size >= queue->max_size)
    return -1;

  buf = malloc(sizeof (struct Node));
  if (buf == NULL)
    err_exit("malloc");
  buf->data = data;
  buf->next = NULL;
  
  if (queue->size == 0)
    queue->front = queue->end = buf;
  else {
    queue->end->next = buf;
    queue->end = buf;
  }
  queue->size++;
  return 0;
}

/*
 * Remove element from front of 'queue' and saving it to 'data'.
 * Return -1 on error and 0 otherwise
 */
int Qpop(struct Queue *queue, int *data) {
  struct Node *next;

  if (queue->size == 0)
    return -1;

  *data = queue->front->data;
  next = queue->front->next;
  free(queue->front);
  queue->front = next;
  if (queue->size == 1)
    queue->end = NULL;  // and equal to queue->front
  queue->size--;
  return 0;
}

/* Deleting of 'queue' */
void Qdelete(struct Queue *queue) {
  struct Node *next;
  
  while ((next = queue->front->next) != NULL)
    free(queue->front);
    queue->front = next;
}
  
/*
 * The function of working with a specific client.
 * Accepts a descriptor of client and sends the current time.
 */
void* handler(void *arg) {
  time_t cur_time;
  int cfd;
  
  /* Waiting for the client */
  while (sem_wait(&sem_num_clients) != -1) {
    if (pthread_mutex_lock(&mut_q_clients) != 0)
      err_exit2("Error: pthread_mutex_lock\n");
    if (Qpop(&q_clients, &cfd) == -1)
      err_exit2("Error: Qpop\n");
    if (pthread_mutex_unlock(&mut_q_clients) != 0)
      err_exit2("Error: pthread_mutex_unlock\n");

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
    if (sem_post(&sem_num_free) == -1)
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

  /* Creating service threads */
  for (int i = 0; i < NUM_SERVING; i++) {
    if (pthread_create(&threads[i], NULL, handler, NULL) != 0)
      err_exit2("Error: pthread_create\n");
  }

  /* Initialization of client request queue */
  if (Qinit(&q_clients, MAX_NUM_CLIENTS) == -1)
    err_exit2("Error: Qinit\n");
  if (pthread_mutex_init(&mut_q_clients, NULL) != 0)
    err_exit2("Error: pthread_mutex_init\n");
  if (sem_init(&sem_num_free, 0, MAX_NUM_CLIENTS) == -1)
    err_exit("sem_init");
  if (sem_init(&sem_num_clients, 0, 0) == -1)
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
    /* Adding next client to the request queue */ 
    if (sem_wait(&sem_num_free) == -1)
      err_exit("sem_wait");

    if (pthread_mutex_lock(&mut_q_clients) != 0)
      err_exit2("Error: pthread_mutex_lock\n");
    if (Qpush(&q_clients, cfd) == -1)
      err_exit2("Error: Qpush\n");
    if (pthread_mutex_unlock(&mut_q_clients) != 0)
      err_exit2("Error: pthread_mutex_unlock\n");
    
    if (sem_post(&sem_num_clients) == -1)
      err_exit("sem_post");

    client_size = sizeof(struct sockaddr_in);
  }
  err_exit("accept");
}
