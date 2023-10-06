#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <netinet/ip.h>

#define err_exit(msg)   do { perror(msg); exit(EXIT_FAILURE); } while(0)

#define BUF_SIZE 1024       /* Size of buffers */
#define SERVER_PORT 8080    /* Port of server */
#define CLIENT_PORT 7777    /* 'Port' of client */

int main(void) {
  struct sockaddr_in server;
  int sfd;
  char message[] = "Hello from client!\n";
  char sbuf[BUF_SIZE], rbuf[BUF_SIZE];
  char *payload;
  struct udphdr *header_udp;
  int server_size;
  unsigned short port;

  /* Creating a socket */
  sfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (sfd == -1)
    err_exit("socket");

  /* Filling the buffer with data and UDP header*/
  memset(sbuf, 1, BUF_SIZE);
  header_udp = (struct udphdr*) sbuf;
  payload = (char *) (sbuf + sizeof(struct udphdr));
  
  /* Filling the payload */
  memcpy(payload, message, sizeof(message));

  /* Filling the UDP header */
  header_udp->source = htons(CLIENT_PORT);
  header_udp->dest = htons(SERVER_PORT);
  header_udp->len = htons(sizeof(struct udphdr) + sizeof(message));
  header_udp->check = 0;

  /* Exchanging messages with the server */
  server.sin_family = AF_INET;
  server.sin_port = 0;
  server.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
  server_size = sizeof(server);
  
  /* Sending message */
  if (sendto(sfd, sbuf, ntohs(header_udp->len), 0,
             (struct sockaddr*) &server, (socklen_t) server_size) == -1)
    err_exit("send");
  printf("Client send: %s", message);

  /* Receiving message */
  port = htons(CLIENT_PORT);
  while (1) {
    server_size = sizeof(server);
    if (recvfrom(sfd, rbuf, BUF_SIZE, 0,
                 (struct sockaddr*) &server, (socklen_t*) &server_size) == -1)
      err_exit("recv");

    header_udp = (struct udphdr*) (rbuf + sizeof(struct iphdr));
    if (memcmp(&header_udp->dest, &port, sizeof(port)) == 0)
      break;
  }
  payload = rbuf + sizeof(struct iphdr) + sizeof(struct udphdr);
  printf("Client receive: %s", payload);

  /* Closing the socket */
  if (close(sfd) == -1)
    err_exit("close");
  return EXIT_SUCCESS;
}
