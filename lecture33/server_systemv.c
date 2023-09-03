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
  
  msqid = msgget(key, IPC_CREAT | 0600);
  if (msqid == -1)
    err_exit("msgget");
  
  smsg.mtype = S_PRIOR;
  snprintf(smsg.mtext, MSGSIZE, "%s\n", "Hello!");
  status = msgsnd(msqid, &smsg, sizeof(smsg.mtext), 0);
  if (status == -1)
    err_exit("msgsnd");
  printf("Server send: %s", smsg.mtext);
  
  status = msgrcv(msqid, &rmsg, sizeof(rmsg.mtext), C_PRIOR, 0);
  if (status == -1)
    err_exit("msgrcv");
  printf("Server recieve: %s", rmsg.mtext);
  
  status = msgctl(msqid, IPC_RMID, NULL);
  if (status == -1)
    err_exit("msgctl");

  return EXIT_SUCCESS;
}
