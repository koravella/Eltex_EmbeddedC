/*
 * A client that creates multiple threads accessing the server at the same time
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

/*
 * Joining the server and getting time from it in an infinite loop
 */
void* requester(void *arg) {
  struct sockaddr_in server;
  int cfd;
  time_t rtime;
  struct tm *cur;

  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = htonl(ADDR);
 
  /* Creating socket */
  cfd = socket(AF_INET, SOCK_STREAM, 0);
  if (cfd == -1)
    err_exit("socket");
/*  
  opt = 1;
  if (setsockopt(cfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) == -1)
    err_exit("setsockopt");
*/
  while (connect(cfd, (struct sockaddr*) &server, sizeof(server)) != -1) {
    /* Receive time from the server */
    if (recv(cfd, &rtime, sizeof(rtime), 0) == -1)
      err_exit("recv");

    /* Closing the connection */
    if (close(cfd) == -1)
      err_exit("close");
    
    /* Creating socket */
    cfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cfd == -1)
      err_exit("socket");
  }
    err_exit("connect");
  }

int main(int argc, char *argv[]) {
  int st;
  int num_requesters;   /* Number of clients */
  pthread_t *clients;
  int wtime;

  if (argc != 3) {
    fprintf(stderr, "Run the program: <bin> <number of clients> <work time (sec)>\n");
    exit(EXIT_FAILURE);
  }
  num_requesters = atoi(argv[1]);
  wtime = atoi(argv[2]);

  /* Launching clients */
  clients = malloc(num_requesters * sizeof(pthread_t));
  for (int i = 0; i < num_requesters; i++)
    if (pthread_create(&clients[i], NULL, requester, NULL) != 0)
      err_exit2("Error: pthread_create\n");

  /* Waiting for a denial of service */
  sleep(wtime);

  /* Termination if no denials occurred */
  for (int i = 0; i < num_requesters; i++)
    if (pthread_cancel(clients[i]) != 0)
      err_exit2("Error: pthread_cancel\n");
  for (int i = 0; i < num_requesters; i++)
    if (pthread_join(clients[i], NULL) != 0)
      err_exit2("Error: pthread_join\n");
  return EXIT_SUCCESS;
}
