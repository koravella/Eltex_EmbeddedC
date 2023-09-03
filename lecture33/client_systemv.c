#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <string.h>
#include "systemv.h"
#include <stdio.h>
#include <sys/msg.h>

#define err_exit(msg)   do { perror(msg); exit(EXIT_FAILURE); } while(0)

int main(void) {
  key_t key;
  int msqid;
  int status;
  struct msgbuf smsg, rmsg;

  key = ftok(BINNAME, 0);
  if (key == -1)
    err_exit("ftok");
  
  msqid = msgget(key, 0);
  if (msqid == -1)
    err_exit("msgget");
   
  status = msgrcv(msqid, &rmsg, sizeof(rmsg.mtext), S_PRIOR, 0);
  if (status == -1)
    err_exit("msgrcv");
  printf("Client recieve: %s", rmsg.mtext);

  smsg.mtype = C_PRIOR;
  snprintf(smsg.mtext, MSGSIZE, "%s\n", "Hi!");
  status = msgsnd(msqid, &smsg, sizeof(smsg.mtext), 0);
  if (status == -1)
    err_exit("msgsnd");
  printf("Client send: %s", smsg.mtext);
  
  return EXIT_SUCCESS;
}
