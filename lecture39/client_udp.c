/*
 * Simple UDP client that connects to server and receives time
 */

#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define err_exit(msg)   do { perror(msg); exit(EXIT_FAILURE); } while(0)

#define PORT_UDP 7777           /* Port of server */
#define ADDR INADDR_LOOPBACK    /* Addres of server */


int main(void) {
  struct sockaddr_in server, client;
  int sfd;
  time_t rtime;
  struct tm *cur;

  /* Creating a socket */
  sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sfd == -1)
    err_exit("socket");

  /* Connecting to server (no real connection) */
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT_UDP);
  server.sin_addr.s_addr = htonl(ADDR);
  if (connect(sfd, (struct sockaddr*) &server, sizeof(server)) == -1)
    err_exit("connect");

  /* Send message to server */
  if (send(sfd, NULL, 0, 0) == -1)
    err_exit("sendto");

  /* Receive time from the server */
  if (recv(sfd, &rtime, sizeof(rtime), 0) == -1)
    err_exit("recv");
  cur = localtime(&rtime);
  printf("Received time: ");
  printf("%d.%d.%d ", cur->tm_mday, cur->tm_mon + 1, cur->tm_year + 1900);
  printf("%d:%d:%d\n", cur->tm_hour, cur->tm_min, cur->tm_sec);

  /* Closing the socket */
  if (close(sfd) == -1)
    err_exit("close");
  return EXIT_SUCCESS;
}
