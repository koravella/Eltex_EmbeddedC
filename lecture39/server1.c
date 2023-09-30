/*
 * A server that creates threads to process each new client.
 */

#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>

/* Macros for termination */
#define err_exit(msg)   do { perror(msg); exit(EXIT_FAILURE); } while(0)
#define err_exit2(msg)  do { fprintf(stderr, msg); \
                             exit(EXIT_FAILURE); } \
                             while(0)

#define PORT 8080               /* Port of server */
#define ADDR INADDR_LOOPBACK    /* Addres of server */
#define LNUM 5                  /* Lenght of queue of clients for listen() */

/*
 * The function of working with a specific client.
 * Accepts a descriptor of client and sends the current time.
 */
void* handler(void *arg) {
  time_t cur_time;
  int cfd;

  cfd = *((int *) arg);
  cur_time = time(NULL);

  if (send(cfd, &cur_time, sizeof(cur_time), 0) == -1) {
    perror("send");
    pthread_exit(0);
  }
  if (close(cfd) == -1) {
    perror("close");
    pthread_exit(0);
  }
  free(arg);
}

int main(void) {
  struct sockaddr_in server;
  struct sockaddr_in client;
  int client_size;
  int lfd;                     /* Listening socket */
  int cfd;                     /* Client socket */
  int *copy_cfd;
  pthread_attr_t attr;
  pthread_t pth;
  int st;
  int opt;

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
  if (pthread_attr_init(&attr) != 0)
    err_exit2("Error: pthread_attr_init");
  if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0)
    err_exit2("Error: pthread_attr_setdetachstate");
  while ((cfd = accept(lfd, (struct sockaddr *) &client, &client_size)) != -1) {
    /* Starting a thread working with the client */
    copy_cfd = (int *) malloc(sizeof(int));
    if (copy_cfd == NULL)
      err_exit("malloc");
    *copy_cfd = cfd;
    if (pthread_create(&pth, &attr, handler, copy_cfd) != 0)
      err_exit2("Error: pthread_create");
    client_size = sizeof(struct sockaddr_in);
  }
  err_exit("accept");
}
