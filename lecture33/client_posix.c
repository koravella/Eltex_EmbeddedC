#include <fcntl.h>
#include <mqueue.h>
#include "posix.h"
#include <stdio.h>
#include <stdlib.h>

#define err_exit(msg)   do { perror(msg); exit(EXIT_FAILURE); } while(0)

int main(void) {
  mqd_t mqdes1; /* Server -> Client */
  mqd_t mqdes2; /* Client -> Server */
  int status;
  char smsg[MSGSIZE], rmsg[MSGSIZE];

  mqdes1 = mq_open(MQNAME1, O_RDONLY);
  if (mqdes1 == (mqd_t) -1)
    err_exit("mq_open");
  mqdes2 = mq_open(MQNAME2, O_WRONLY);
  if (mqdes2 == (mqd_t) -1)
    err_exit("mq_open");
  
  status = mq_receive(mqdes1, rmsg, MSGSIZE, NULL);
  if (status == -1)
    err_exit("mq_receive");
  printf("Client receive: %s", rmsg);

  snprintf(smsg, MSGSIZE, "%s\n", "Hi!");
  status = mq_send(mqdes2, smsg, sizeof(smsg), 1);
  if (status == -1)
    err_exit("mq_send");
  printf("Client send: %s", smsg); 

  status = mq_close(mqdes1);
  if (status == -1)
    err_exit("mq_close");
  status = mq_close(mqdes2);
    if (status == -1)
      err_exit("mq_close");
  
  return EXIT_SUCCESS;
}
