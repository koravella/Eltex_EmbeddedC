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

#define BUF_SIZE 1024         /* Size of buffers */
#define SERVER_PORT 8080      /* Port of server */
#define CLIENT_PORT 7777      /* 'Port' of client */
#define ADDR INADDR_LOOPBACK  /* Address of server */

int main(void) {
  struct sockaddr_in server;
  int sfd;
  char message[] = "Hello from client!\n";
  char sbuf[BUF_SIZE];        /* Send buffer */
  char rbuf[BUF_SIZE];        /* Receive buffer */
  char *payload;
  struct udphdr *header_udp;
  struct iphdr *header_ip;
  int server_size;
  unsigned short port;
  int opt;
  int buf_size;

  /* Creating a socket */
  sfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (sfd == -1)
    err_exit("socket");
  
  opt = 1;
  if (setsockopt(sfd, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt)) == -1)
    err_exit("setsockopt");

  /* Filling the buffer with data and UDP header*/
  memset(sbuf, 0, BUF_SIZE);  //zeroing of buffer
  header_ip = (struct iphdr*) sbuf;
  header_udp = (struct udphdr*) (sbuf + sizeof(struct iphdr));
  payload = (char *) (sbuf + sizeof(struct iphdr) + sizeof(struct udphdr));
  
  /* Filling the payload */
  memcpy(payload, message, sizeof(message));

  /* Filling the UDP header */
  header_udp->source = htons(CLIENT_PORT);
  header_udp->dest = htons(SERVER_PORT);
  header_udp->len = htons(sizeof(struct udphdr) + sizeof(message));
  header_udp->check = 0;

  /* Filling the IP header*/
  buf_size = sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(message);
  header_ip->ihl = 5;
  header_ip->version = 4;
  header_ip->tos = 0;
  header_ip->tot_len = htons(buf_size); // always filled in
  header_ip->id = 0;  // filled in
  header_ip->frag_off = 0x40; // don't fragment
  header_ip->ttl = 128;
  header_ip->protocol = IPPROTO_UDP;
  header_ip->check = 0; //always filled in
  header_ip->saddr = 0; // filled in
  header_ip->daddr = htonl(ADDR);

  /* Exchanging messages with the server */
  server.sin_family = AF_INET;
  server.sin_port = 0;
  server.sin_addr.s_addr = htonl(ADDR); 
  server_size = sizeof(server);
  
  /* Sending message */
  buf_size = sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(message);
  if (sendto(sfd, sbuf, buf_size, 0,
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
