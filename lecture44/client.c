#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <netinet/ether.h>
#include <net/if.h>

#define err_exit(msg)   do { perror(msg); exit(EXIT_FAILURE); } while(0)

#define BUF_SIZE 1024                 /* Size of buffers */
#define SERVER_PORT 8080              /* Port of server */
#define CLIENT_PORT 7777              /* 'Port' of client */
//#define server_addr "192.168.0.103"
//#define client_addr "192.168.0.102"
//#define src_mac "84:7b:57:76:97:06"
//#define dest_mac "3c:7c:3f:d5:cd:08"
//#define name_interface "wlo1"

short checksum (void *buf) {
  short *ptr;
  int csum;
  int over;

  ptr = (short*) buf;
  csum = 0;
  for (int i = 0; i < 10; i++) {
    csum += ntohs(*ptr);
    ptr++;
  }
  over = csum >> 16;
  csum = (csum & 0xFFFF) + over;
  csum = ~csum;
  return htons((short) csum);
}

int main(int argc, char *argv[]) {
  struct sockaddr_ll server;
  int sfd;
  char message[] = "Hello from client!\n";
  char sbuf[BUF_SIZE];        /* Send buffer */
  char rbuf[BUF_SIZE];        /* Receive buffer */
  char *payload;
  struct udphdr *header_udp;
  struct iphdr *header_ip;
  struct ether_header *header_eth;
  int server_size;
  unsigned short port;
  int buf_size;
  struct ether_addr *addr_eth;
  char *server_addr, *client_addr;
  char *src_mac, *dest_mac;
  char *name_interface;

  if (argc != 6) {
    fprintf(stderr, "Usage: <bin> <server address> <client address>\n\
       <source mac> <destination mac> <name of interface>\n");
    exit(EXIT_FAILURE);
  }
  server_addr = argv[1];
  client_addr = argv[2];
  src_mac = argv[3];
  dest_mac = argv[4];
  name_interface = argv[5];

  /* Creating a socket */
  sfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (sfd == -1)
    err_exit("socket");
  
  /* Filling the buffer with data, UDP header, IP header and link-layer header*/
  memset(sbuf, 0, BUF_SIZE);  //zeroing of buffer
  header_eth = (struct ether_header*) sbuf;
  header_ip = (struct iphdr*) (sbuf + sizeof(struct ether_header));
  header_udp = (struct udphdr*) (sbuf + sizeof(struct iphdr) +
                                 sizeof(struct ether_header));
  payload = (char *) (sbuf + sizeof(struct iphdr) + sizeof(struct udphdr) +
                      sizeof(struct ether_header));
  
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
  header_ip->tot_len = htons(sizeof(struct iphdr) +
                             sizeof(struct udphdr) +
                             sizeof(message));
  header_ip->id = 0;
  header_ip->frag_off = 0x40; // don't fragment
  header_ip->ttl = 128;
  header_ip->protocol = IPPROTO_UDP;
  header_ip->check = 0;
  header_ip->saddr = inet_addr(client_addr);
  header_ip->daddr = inet_addr(server_addr);
  header_ip->check = checksum(header_ip);

  /* Filling the ethernet header */
  addr_eth = ether_aton(src_mac);
  for (int i = 0; i < 6; i++)
    header_eth->ether_shost[i] = ((uint8_t *) addr_eth)[i];
  addr_eth = ether_aton(dest_mac);
  for (int i =0; i <6; i++)
    header_eth->ether_dhost[i] = ((uint8_t *) addr_eth)[i];
  header_eth->ether_type = htons(ETHERTYPE_IP);

  /* Exchanging messages with the server */
  server.sll_family = AF_PACKET;
  server.sll_ifindex = if_nametoindex(name_interface);
  server.sll_halen = ETHER_ADDR_LEN;
  addr_eth = ether_aton(dest_mac);
  for (int i = 0; i < 6; i++)
    server.sll_addr[i] = ((unsigned char *) addr_eth)[i]; 
  server_size = sizeof(server);
  
  /* Sending message */
  buf_size = sizeof(struct ether_header) + sizeof(struct iphdr) +
             sizeof(struct udphdr) + sizeof(message);
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

    header_udp = (struct udphdr*) (rbuf + sizeof(struct iphdr) +
                                          sizeof(struct ether_header));
    if (memcmp(&header_udp->dest, &port, sizeof(port)) == 0)
      break;
  }
  payload = rbuf + sizeof(struct iphdr) + sizeof(struct udphdr) +
                   sizeof(struct ether_header);
  printf("Client receive: %s", payload);

  /* Closing the socket */
  if (close(sfd) == -1)
    err_exit("close");
  return EXIT_SUCCESS;
}
