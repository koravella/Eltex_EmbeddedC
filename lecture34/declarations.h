/*
 * Сommon definitions to server and client.
 */

#ifndef DECLARATIONS_H
#define DECLARATIONS_H

#include <sys/types.h>

#define USERNAME_SIZE 20  /* The max size of the nickname with '\0' */
#define TEXT_SIZE 256     /* The max size of text field in Message with '\0' */
#define MAX_USERS 20      /* The max number of users */

#define SHMNAME "/shared_messages"  /* Name of shared memory segment */
#define SEMNAME "/semaph"   /* Name of semaphore for access to shared memory */

#define MAX_MESSAGES 1024   /* Max number of messages in shared memory */

/* Types of messages */
enum Type {
  TEXT,           /* Text message from sender */
  USER_ONLINE,            
  USER_OFFLINE,
  STOP            /* Server stopped */           
};

/* Universal structure for messages */
struct Message {
  enum Type type;                 /* Type of message */
  char username[USERNAME_SIZE];   /* Username of sender */
  char text[TEXT_SIZE];           /* Data field */
};

/* Possible status of user */
enum Userstatus {
  ONLINE,
  OFFLINE,
};

/* Information about user */
struct User {
  char username[USERNAME_SIZE];   
  enum Userstatus userstatus;     /* Status of user */
};

/* Non-expandable buffer for saving messages and users */
struct Bufmsg {
  size_t num_users;     /* Current num of users */
  size_t num_msgs;      /* Current num of messages */
  struct User users[MAX_USERS];
  struct Message msgs[MAX_MESSAGES];
};

#endif
