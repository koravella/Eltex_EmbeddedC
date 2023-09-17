#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define err_exit(msg)   do { perror(msg); exit(EXIT_FAILURE); } while(0)

#define BUF_SIZE 1024           /* Size of recieve buffer */
#define SOCK_PATH "/tmp/socket_ls"  /* Name for socket */

int main(void) {
  struct sockaddr_un server;
  int fd_connect;
  char message[] = "Hello from client!\n";
  char buf[BUF_SIZE];

  /* Connecting to the server */ 
  fd_connect = socket(AF_LOCAL, SOCK_STREAM, 0);
  if (fd_connect == -1)
    err_exit("socket");

  server.sun_family = AF_LOCAL;
  strncpy(server.sun_path, SOCK_PATH, sizeof(server.sun_path) -1);
  if (connect(fd_connect, (struct sockaddr*) &server, sizeof(server)) == -1)
    err_exit("connect");

  /* Exchanging messages with the server */
  if (send(fd_connect, message, sizeof(message), 0) == -1)
    err_exit("send");
  printf("Client send: %s", message);

  if (recv(fd_connect, buf, BUF_SIZE, 0) == -1)
    err_exit("recv");    
  printf("Client receive: %s", buf);

  /* Closing the connection */
  if (close(fd_connect) == -1)
    err_exit("close");
  return EXIT_SUCCESS;
}
