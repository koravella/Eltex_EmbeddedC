#include "declarations.h"
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>

#define INIT_MSG_BUF_SIZE 256 /* Initial size of text message buffer on server*/

#define MAXMSG 128    /* Max number of messages in SystemV message queue */
#define MSGSIZE 1024  /* Max size of messages in SystemV message queue */

/* Macros for early termination */
#define err_exit_pth(msg) do {perror(msg); \
                              printf("Server stopped, please quit\n"); \
                              pthread_exit(NULL); \
                              } while(0)
#define err_exit2(msg)  do { fprintf(stderr, msg); exit(EXIT_FAILURE);} while(0)
#define err_exit2_pth(msg)  do {fprintf(stderr, msg); \
                                printf("Server stopped, please quit\n"); \
                                pthread_exit(NULL); \
                                } while(0)

/* Server buffer for text messages, can be expanded */
struct Bufmsg {
  size_t size;
  size_t maxsize;
  struct Message *msgs;
};


struct Bufmsg* New_Bufmsg(void) {
  struct Bufmsg *buf;
  
  buf = malloc(sizeof(struct Bufmsg));
  if (buf == NULL) {
    perror("malloc");
    return NULL;
  }
  buf->maxsize = INIT_MSG_BUF_SIZE;
  buf->msgs = malloc(sizeof(struct Message) * buf->maxsize);
  if (buf->msgs == NULL) {
    perror("malloc");
    free(buf);
    return NULL;
  }
  buf->size = 0;
  return buf;
}

void Delete_Bufmsg(struct Bufmsg *buf) {
  free(buf->msgs);
  free(buf);
}

/* Expand by 2 times */
int Expand_Bufmsg(struct Bufmsg *buf) {
  buf->msgs = realloc(buf->msgs, buf->maxsize * 2);
  if (buf->msgs == NULL) {
    perror("realloc");
    return -1;
  }
  buf->maxsize *= 2;
  return 0;
}

/* Thread cancellation clean-up handler */
void Cleanup_Bufmsg(void *buf) {
  Delete_Bufmsg(buf);
}

/* Thread cancellation clean-up handler */
void Cleanup_queue(void *msqid) {
  int *msq = (int*) msqid;

  msgctl(*msq, IPC_RMID, NULL);
}

/*
 * Return -1 if 'username' is not in users[] and index the first index of
 * occurrence otherwise
 */
int Registred(struct User users[], size_t size, char username[]) {
  for (size_t i = 0; i < size; i++)
    if (strcmp(username, users[i].username) == 0)
      return i;
  return -1;
}

/* Wrapper over 'msgsnd', collecting 'msg' and 'type' in 'struct msgbuf' */
void Send(int msqid, struct Message *msg, size_t msgsz, int msgflg, int type) {
  struct msgbuf smsgbuf;
  int status;

  smsgbuf.mtype = type;
  smsgbuf.data = *msg;
  status = msgsnd(msqid, &smsgbuf, msgsz, msgflg);
    if (status == -1)
      err_exit_pth("msgsnd");
}

/* Function of server for thread */
void* Server(void* arg) {
  int num_users;                 /* Number of registred users */
  struct User users[MAX_USERS];  /* Registred users */
  struct Bufmsg *mbuf;           /* Buffer for all text messages */
  int msqSC;                     /* Server -> Client */
  int msqCS;                     /* Client -> Server */
  key_t keySC, keyCS;
  int status;
  struct msgbuf rmsgbuf, trash;
  int k;

  num_users = 0;
  for (size_t i = 0; i < MAX_USERS; i++)
    users[i].userstatus = EMPTY;
  
  keySC = ftok(BINNAME, KEYSC);
    if (keySC == -1)
      err_exit_pth("ftok");
  keyCS = ftok(BINNAME, KEYCS);
    if (keyCS == -1)
      err_exit_pth("ftok");

  /* Memory allocation and set thread clean-up hadlers */
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
  
  mbuf = New_Bufmsg();
  if (mbuf == NULL)
    err_exit2_pth("Error: New_Bufmsg");
  pthread_cleanup_push(Cleanup_Bufmsg, mbuf);
  
  msqSC = msgget(keySC, IPC_CREAT | 0600);
    if (msqSC == -1)
      err_exit_pth("msgget");
  pthread_cleanup_push(Cleanup_queue, &msqSC);
  
  msqCS = msgget(keyCS, IPC_CREAT | 0600);
    if (msqCS == -1)
      err_exit_pth("msgget");
  pthread_cleanup_push(Cleanup_queue, &msqCS);
  
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

  while((status = msgrcv(msqCS, &rmsgbuf, sizeof(struct Message),0,0)) != -1) {
    struct Message rmsg, smsg;  /* Received and sent messages respectively */
    
    rmsg = rmsgbuf.data;
    switch (rmsg.type) {
      
      /* Processing a registration request */
      case REGISTRATION_REQUEST:
        if ((k = Registred(users, MAX_USERS, rmsg.username)) == -1) {
          /* New user */
          if (num_users < MAX_USERS) {
            users[num_users].upid = rmsg.spid;
            strncpy(users[num_users].username, rmsg.username, USERNAME_SIZE);
            users[num_users].userstatus = ONLINE;
            num_users++;
            smsg.type = REGISTRATION_ACCEPTED;
          }
          else {
            smsg.type = REGISTRATION_REJECTED;
            sprintf(smsg.text, "Too many users\n");
          }
        }
        else {
          /* Such user is already registred */
          if (users[k].userstatus == OFFLINE) {
            smsg.type = REGISTRATION_ACCEPTED;
            users[k].upid = rmsg.spid;
            users[k].userstatus = ONLINE;
          }
          else { /* users[k].userstatus == ONLINE */
            smsg.type = REGISTRATION_REJECTED;
            sprintf(smsg.text, "Such a user is already online\n");
          }
        }

        /* Sending a response to the requester */
        Send(msqSC, &smsg, sizeof(struct Message), 0, rmsg.spid);
        
        if (smsg.type == REGISTRATION_ACCEPTED) {
          /* Sending list of Registred users to new user */
          for (size_t i = 0; i < num_users; i++) {
            smsg.type = users[i].userstatus == ONLINE ? USER_ONLINE : USER_OFFLINE;
            strncpy(smsg.username, users[i].username, USERNAME_SIZE);
            Send(msqSC, &smsg, sizeof(struct Message), 0, rmsg.spid);
          }

          /* Sending notification about new user to all */
          smsg.type = USER_ONLINE;
          strncpy(smsg.username, rmsg.username, USERNAME_SIZE);
          for (size_t i = 0; i<num_users; i++){
            if (users[i].userstatus == ONLINE)
              Send(msqSC, &smsg, sizeof(struct Message), 0, users[i].upid);
          }
                  
          /* Sending all previous messages to new user */
          for (size_t i = 0; i < mbuf->size; i++) {
            Send(msqSC, &(mbuf->msgs[i]), sizeof(struct Message),0,rmsg.spid);
          }
        }
        break;
      
      /* Processing a new text message */
      case TEXT:
        smsg.type = TEXT;
        strncpy(smsg.username, rmsg.username, USERNAME_SIZE);
        strncpy(smsg.text, rmsg.text, TEXT_SIZE);

        if (mbuf->size == mbuf->maxsize) {
          status = Expand_Bufmsg(mbuf) == -1;
          if (status == -1)
            err_exit2_pth("Expand_Bufmsg");
        }
        mbuf->msgs[mbuf->size] = smsg;
        mbuf->size++;

        /* Sending new text message to all users */
        for (size_t i = 0; i<num_users; i++) {
          if (users[i].userstatus == ONLINE)
            Send(msqSC, &smsg, sizeof(struct Message), 0, users[i].upid);
        }
        break;
      
      /* Processing user disconnection notification */
      case EXIT:
        if ((k = Registred(users, MAX_USERS, rmsg.username)) != -1) {
          users[k].userstatus = OFFLINE;
                    
          /* Deleting messages for this user from the queue */
          while (msgrcv(msqSC, &trash, sizeof(struct Message),
                        rmsg.spid, IPC_NOWAIT) != -1)
            if (errno != ENOMSG)
              err_exit_pth("msgrcv");

          /* Sending a user exit notification to all */
          smsg.type = USER_OFFLINE;
          strncpy(smsg.username, rmsg.username, USERNAME_SIZE);
          for (size_t i = 0; i<num_users; i++)
            if (users[i].userstatus == ONLINE)
              Send(msqSC, &smsg, sizeof(struct Message), 0, users[i].upid);
        }
        break;

      default:
        break;
    }
  }
  if (status == -1)
    err_exit_pth("msgrcv");
  pthread_cleanup_pop(1);
  pthread_cleanup_pop(1);
  pthread_cleanup_pop(1);
}

int main(void) {
  static struct termios oldt, newt;
  int status;
  pthread_t thread;

  /* Starting the server */
  status = pthread_create(&thread, NULL, Server, NULL);
  if (status != 0)
    err_exit2("Error: pthread_create");
  printf("Server started\n");

  /* Waiting for the finalisation command */
  printf("Enter 'q' to stop server and quit\n");
  tcgetattr( STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr( STDIN_FILENO, TCSANOW, &newt);
  while (getchar() != 'q');
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt);

  /* Stopping the server */
  status = pthread_cancel(thread);
  if (status != 0)
    err_exit2("Error: pthread_cancel");
  status = pthread_join(thread, NULL);
  if (status != 0)
    err_exit2("Error: pthread_join");
  return EXIT_SUCCESS;
}
