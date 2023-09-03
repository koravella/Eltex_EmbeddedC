#ifndef SYSTEMV_H
#define SYSTEMV_H

#define BINNAME "server_systemv.out"  /* Pathname for ftok */
#define MSGSIZE 128                   /* Max size of buffer for message */
#define S_PRIOR 1                     /* Priority for messages from server */
#define C_PRIOR 2                     /* Priority for messages from client */

/* structure for SystemV messages */
struct msgbuf {
  long mtype;
  char mtext[MSGSIZE];
};

#endif
