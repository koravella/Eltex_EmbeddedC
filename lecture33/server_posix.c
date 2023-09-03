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
  struct mq_attr attr1, attr2;;
  char smsg[MSGSIZE], rmsg[MSGSIZE];

  attr1.mq_maxmsg = attr2.mq_maxmsg = MAXMSG;
  attr1.mq_msgsize = attr2.mq_msgsize = MSGSIZE;

  mqdes1 = mq_open(MQNAME1, O_WRONLY | O_CREAT, 0600, &attr1);
  if (mqdes1 == (mqd_t) -1)
    err_exit("mq_open");
  mqdes2 = mq_open(MQNAME2, O_RDONLY | O_CREAT, 0600, &attr2);
  if (mqdes2 == (mqd_t) -1)
    err_exit("mq_open");

  snprintf(smsg, MSGSIZE, "%s\n", "Hello!");
  status = mq_send(mqdes1, smsg, sizeof(smsg), 1);
  if (status == -1)
    err_exit("mq_send");
  printf("Server send: %s", smsg); 

  status = mq_receive(mqdes2, rmsg, MSGSIZE, NULL);
  if (status == -1)
    err_exit("mq_receive");
  printf("Server receive: %s", rmsg);

  status = mq_close(mqdes1);
  if (status == -1)
    err_exit("mq_close");
  status = mq_close(mqdes2);
    if (status == -1)
      err_exit("mq_close");
  
  status = mq_unlink(MQNAME1);
    if (status == -1)
      err_exit("mq_unlink");

  status = mq_unlink(MQNAME2);
    if (status == -1)
      err_exit("mq_unlink");

return EXIT_SUCCESS;
}
