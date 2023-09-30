/*
 * A server that can procces TCP and UDP connection simultaneously using poll
 */

#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <poll.h>

/* Macros for termination */
#define err_exit(msg)   do { perror(msg); exit(EXIT_FAILURE); } while(0)

#define PORT_TCP 8080           /* Port of server for TCP-connection */
#define PORT_UDP 7777           /* Port of serverfor UDP-connection */
#define ADDR INADDR_LOOPBACK    /* Addres of server */
#define LNUM 5                  /* Lenght of queue of clients for listen() */

int main(void) {
  struct sockaddr_in server;
  struct sockaddr_in client;
  int client_size;
  int tcp_lfd;                /* TCP listening  socket */
  int udp_fd;                 /* UDP socket */
  int fd;                    
  pthread_attr_t attr;
  int st;
  int opt;
  struct pollfd pfds[2];      /* 0 -- for TCP, 1 -- for UDP */
  time_t cur_time;

  /* Creating a TCP listening socket */
  tcp_lfd = socket(AF_INET, SOCK_STREAM, 0);
  if (tcp_lfd == -1)
    err_exit("socket");
  
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT_TCP);
  server.sin_addr.s_addr = htonl(ADDR);
  
  opt = 1;
  if (setsockopt(tcp_lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) == -1)
    err_exit("setsockopt");

  if (bind(tcp_lfd, (struct sockaddr*) &server, sizeof(server)) == -1)
    err_exit("bind");

  if (listen(tcp_lfd, LNUM) == -1)
    err_exit("listen");

  /* Creating a UDP socket */
  udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (udp_fd == -1)
    err_exit("socket");

  server.sin_family = AF_INET;
  server.sin_port = htons(PORT_UDP);
  server.sin_addr.s_addr = htonl(ADDR);
  
  if (bind(udp_fd, (struct sockaddr*) &server, sizeof(server)) == -1)
    err_exit("bind");

  /* Registration for poll */
  pfds[0].fd = tcp_lfd;
  pfds[0].events = POLLIN;

  pfds[1].fd = udp_fd;
  pfds[1].events = POLLIN;

  /* Processing clients: infinity loop */
  while (poll(pfds, 2, -1) != -1) {
    cur_time = time(NULL);

    /* TCP socket */
    if (pfds[0].revents & POLLIN) {
      client_size = sizeof(struct sockaddr_in);
      fd = accept(tcp_lfd, (struct sockaddr *) &client, &client_size);
      if (fd == -1)
        err_exit("accept");
      
      if (send(fd, &cur_time, sizeof(cur_time), 0) == -1)
        err_exit("send");
      
      if (close(fd) == -1)
        err_exit("close");
    }
    
    /* UDP socket */
    if (pfds[1].revents & POLLIN) {
      client_size = sizeof(struct sockaddr_in);
      int tmp;
      if (recvfrom(udp_fd, NULL, 0, 0, (struct sockaddr*) &client,
                   (socklen_t*) &client_size) == -1)
        err_exit("recvfrom");

      if (sendto(udp_fd, &cur_time, sizeof(cur_time), 0,
                 (struct sockaddr*) &client, (socklen_t) client_size) == -1)
        err_exit("sendto");
    }
  }
  err_exit("poll");
}
