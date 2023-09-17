#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define err_exit(msg)   do { perror(msg); exit(EXIT_FAILURE); } while(0)

#define BUF_SIZE 1024                           /* Size of recieve buffer */
#define SERV_SOCK_PATH "/tmp/serv_socket_ld"  /* Name for server socket */
#define CLIE_SOCK_PATH "/tmp/clie_socket_ld"  /* Name for client socket */

int main(void) {
  struct sockaddr_in server, client;
  int sfd;
  char message[] = "Hello from client!\n";
  char buf[BUF_SIZE];

  /* Creating a socket */
  sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sfd == -1)
    err_exit("socket");

  /* Connecting to server (no real connection) */
  server.sin_family = AF_INET;
  server.sin_port = htons(8080);
  server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  if (connect(sfd, (struct sockaddr*) &server, sizeof(server)) == -1)
    err_exit("connect");

  /* Exchanging messages with the server */
  if (send(sfd, message, sizeof(message), 0) == -1)
    err_exit("send");
  printf("Client send: %s", message);

  if (recv(sfd, buf, BUF_SIZE, 0) == -1)
    err_exit("recv");    
  printf("Client receive: %s", buf);

  /* Closing the socket */
  if (close(sfd) == -1)
    err_exit("close");
  return EXIT_SUCCESS;
}
