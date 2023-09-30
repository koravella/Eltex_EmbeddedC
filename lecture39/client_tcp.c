/*
 * Simple TCP client that connects to server and receives time
 */

#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>

#define err_exit(msg)   do { perror(msg); exit(EXIT_FAILURE); } while(0)

#define PORT_TCP 8080           /* Port of server */
#define ADDR INADDR_LOOPBACK    /* Addres of server */

int main(void) {
  struct sockaddr_in server;
  int cfd;
  time_t rtime;
  struct tm *cur;

  /* Connecting to the server */ 
  cfd = socket(AF_INET, SOCK_STREAM, 0);
  if (cfd == -1)
    err_exit("socket");

  server.sin_family = AF_INET;
  server.sin_port = htons(PORT_TCP);
  server.sin_addr.s_addr = htonl(ADDR);
  if (connect(cfd, (struct sockaddr*) &server, sizeof(server)) == -1)
    err_exit("connect");

  /* Receive time from the server */
  if (recv(cfd, &rtime, sizeof(rtime), 0) == -1)
    err_exit("recv");
  cur = localtime(&rtime);
  printf("Received time: ");
  printf("%d.%d.%d ", cur->tm_mday, cur->tm_mon + 1, cur->tm_year + 1900);
  printf("%d:%d:%d\n", cur->tm_hour, cur->tm_min, cur->tm_sec);

  /* Closing the connection */
  if (close(cfd) == -1)
    err_exit("close");
  return EXIT_SUCCESS;
}
