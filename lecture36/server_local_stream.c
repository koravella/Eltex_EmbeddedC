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
  struct sockaddr_un server, client;
  int client_size;
  int fd_listener, fd_connect;
  char message[] = "Hello from server!\n";
  char buf[BUF_SIZE];

  /* Creating a listening socket */ 
  fd_listener = socket(AF_LOCAL, SOCK_STREAM, 0);
  if (fd_listener == -1)
    err_exit("socket");

  server.sun_family = AF_LOCAL;
  strncpy(server.sun_path, SOCK_PATH, sizeof(server.sun_path) -1);
  if (bind(fd_listener, (struct sockaddr*) &server, sizeof(server)) == -1)
    err_exit("bind");

  if (listen(fd_listener, 1) == -1)
    err_exit("listen");

  /* Connecting the client */ 
  client_size = sizeof(client);
  fd_connect = accept(fd_listener, (struct sockaddr *) &client, &client_size);
  if (fd_connect == -1)
    err_exit("accept");

  /* Exchanging messages with the client */
  if (send(fd_connect, message, sizeof(message), 0) == -1)
    err_exit("send");
  printf("Server send: %s", message);

  if (recv(fd_connect, buf, BUF_SIZE, 0) == -1)
    err_exit("recv");    
  printf("Server receive: %s", buf);

  /* Closing connections */
  if (close(fd_connect) == -1)
    err_exit("close");
  if (close(fd_listener) == -1)
    err_exit("close");
  if (unlink(SOCK_PATH) == -1)
    err_exit("unlink");
  return EXIT_SUCCESS;
}
