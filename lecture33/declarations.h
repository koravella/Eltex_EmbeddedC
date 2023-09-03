#ifndef DECLARATIONS_H
#define DECLARATIONS_H

#include <sys/types.h>

#define USERNAME_SIZE 20  /* The max size of the nickname with '\0' */
#define TEXT_SIZE 256     /* The max size of text field in Message with '\0' */
#define MAX_USERS 20      /* The max number of users */

#define BINNAME "server.out"  /* Name for ftok() */
#define KEYSC 0               /* Value for ftok() */
#define KEYCS 1               /* Value for ftok() */

#define PR_RREQ 3   /* Priority for registration */ 
#define PR_TEXT 1   /* Priority for text messages */
#define PR_EXIT 4   /* Priority for exit notification */

/* Types of messages to server and from server */
enum Type {
  REGISTRATION_REQUEST,   
  REGISTRATION_ACCEPTED,  
  REGISTRATION_REJECTED,  
  TEXT,                   /* Text message from sender */
  USER_ONLINE,            
  USER_OFFLINE,           
  EXIT                    /* Sender logged out */
};

/* Universal structure for messages */
struct Message {
  enum Type type;                 /* Type of message */
  pid_t spid;                     /* pid of sender */
  char username[USERNAME_SIZE];   /* Username of sender */
  char text[TEXT_SIZE];           /* Data field */
};

/* Possible status of user */
enum Userstatus {
  ONLINE,
  OFFLINE,
  EMPTY     /* User does not exist */
};

/* Information about user */
struct User {
  pid_t upid;                     /* pid of user's client program */
  char username[USERNAME_SIZE];   
  enum Userstatus userstatus;     /* Status of user */
};

/* Structure for SystemV messages */
struct msgbuf {
  long mtype;
  struct Message data;
};

#endif
