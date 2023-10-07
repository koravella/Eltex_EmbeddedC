#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define err_exit(msg)   do { perror(msg); exit(EXIT_FAILURE); } while(0)

#define BUF_SIZE 1024           /* Size of recieve buffer */
#define SERVER_PORT 8080

int main(void) {
  struct sockaddr_in server, client;
  int client_size;
  int sfd;
  char message[] = "Hello from server!\n";
  char buf[BUF_SIZE];

  /* Creating a socket */ 
  sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sfd == -1)
    err_exit("socket");

  server.sin_family = AF_INET;
  server.sin_port = htons(SERVER_PORT);
  server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  if (bind(sfd, (struct sockaddr*) &server, sizeof(server)) == -1)
    err_exit("bind");

  /* Exchanging messages with the client */
  client_size = sizeof(client);
  if (recvfrom(sfd, buf, BUF_SIZE, 0,
               (struct sockaddr*) &client, (socklen_t*) &client_size) == -1)
    err_exit("recvfrom");
  printf("Server receive: %s", buf);

  if (sendto(sfd, message, sizeof(message), 0,
             (struct sockaddr*) &client, (socklen_t) client_size) == -1)
    err_exit("sendto");
  printf("Server send: %s", message);

  /* Closing the sockets */
  if (close(sfd) == -1)
    err_exit("close");
  return EXIT_SUCCESS;
}
