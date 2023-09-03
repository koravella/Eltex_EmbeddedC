#include "declarations.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <curses.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>

/* Minimal required size of terminal */
#define MIN_COLS 80
#define MIN_LINES 25

#define HEIGTH_INPUT 4  /* Size of input window */
#define WIDTH_USERS 19  /* Size of the user display window */

/* Macros for early termination */
#define err_exit(msg)   do { perror(msg); exit(EXIT_FAILURE); } while(0)
#define err_exit2(msg)  do { endwin(); \
                             fprintf(stderr, msg); \
                             exit(EXIT_FAILURE); } while(0)
#define err_exit_pth    do { pthread_exit(NULL); } while(0)

/* Integer rounding up x/y */
#define ceill(x, y) ((x + y - 1) / y)

/* Semaphore for controlling thread termination */
sem_t sem;

/* Wrapper over 'msgsnd', collecting 'msg' and 'type' in 'struct msgbuf' */
int Send(int msqid, struct Message *msg, size_t msgsz, int msgflg, int type) {
  struct msgbuf smsgbuf;
  int status;

  smsgbuf.mtype = type;
  smsgbuf.data = *msg;
  status = msgsnd(msqid, &smsgbuf, msgsz, msgflg);
  return status;
}

/* Thread cancellation clean-up handler (unlock semaphor) */
void Unlock_sem(void *sem) {
  sem_post(sem);
}

/* Thread cancellation clean-up handler */
void Delete_window(void *wnd) {
  delwin(wnd);
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

/* Print 'username' on line 'line' in user display window */
void Print_user(WINDOW* wnd, int line, char username[]) {
  mvwaddstr(wnd, line, 0, username);
  wrefresh(wnd);
}

/* Like Print() but hightlighted*/
void Print_hightlight_user(WINDOW* wnd, int line, char username[]) {
  wattron(wnd, A_REVERSE);
  Print_user(wnd, line, username);
  wattroff(wnd, A_REVERSE);
}

/* Function of reader message queue for thread */
void* Thread_reader(void* arg) {
  struct User users[MAX_USERS]; /* Registred users */
  int num_users;                /* Number of registred users */
  int msqSC;                    /* Server -> Client */
  key_t keySC;
  int status;
  struct msgbuf rmsgbuf;
  struct Message rmsg;
  WINDOW *wnd_msgs;             /* Window for displaying text messages */
  WINDOW *wnd_users;            /* Window for displaying registred users */
  int k;
  pid_t pid;                    /* pid of current program */
  int nlines;
  int xmax_msgs, ymax_msgs;

  /* Memory allocation and set thread clean-up hadlers */
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL); 
  pthread_cleanup_push(Unlock_sem, &sem);

  wnd_msgs = newwin(LINES - 1 - HEIGTH_INPUT, COLS - 1 - WIDTH_USERS, 0, 0);
  pthread_cleanup_push(Delete_window, wnd_msgs);
  wnd_users = newwin(LINES - 1 - HEIGTH_INPUT,WIDTH_USERS,0,COLS - WIDTH_USERS);
  pthread_cleanup_push(Delete_window, wnd_users);
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  
  scrollok(wnd_msgs, TRUE);
  werase(wnd_msgs);
  wrefresh(wnd_msgs);
  getmaxyx(wnd_msgs, ymax_msgs, xmax_msgs);
  pid = getpid();
  num_users = 0;
  for (size_t i = 0; i < MAX_USERS; i++)
    users[i].userstatus = EMPTY;

  keySC = ftok(BINNAME, KEYSC);
    if (keySC == -1)
      err_exit_pth;
  msqSC = msgget(keySC, 0);
    if (msqSC == -1)
      err_exit_pth;


  while((status = msgrcv(msqSC, &rmsgbuf,sizeof(struct Message),pid,0)) != -1) {
    rmsg = rmsgbuf.data; /* received message */

    switch(rmsg.type) {

      /* Processing a new text message */
      case TEXT:
        nlines = ceill(USERNAME_SIZE + strlen(rmsg.text), xmax_msgs); 
        if (nlines > ymax_msgs)
          nlines = ymax_msgs;
        wscrl(wnd_msgs, nlines);
        mvwprintw(wnd_msgs, ymax_msgs - nlines, 0, "%s: %s",
                  rmsg.username, rmsg.text);
        wrefresh(wnd_msgs);
        break;

      /* Processing a notification about online user */
      case USER_ONLINE:
        if ((k = Registred(users, MAX_USERS, rmsg.username)) == -1) {
          strncpy(users[num_users].username, rmsg.username, USERNAME_SIZE);
          users[num_users].userstatus = ONLINE;
          num_users++;
          Print_hightlight_user(wnd_users, num_users - 1, rmsg.username);
        }
        else {
          users[k].userstatus = ONLINE;
          Print_hightlight_user(wnd_users, k, rmsg.username);
        }
        break;
      
      /* Processing a notification about offline user */
      case USER_OFFLINE:
        if ((k = Registred(users, MAX_USERS, rmsg.username)) == -1) {
          strncpy(users[num_users].username, rmsg.username, USERNAME_SIZE);
          users[num_users].userstatus = OFFLINE;
          num_users++;
          Print_user(wnd_users, num_users - 1, rmsg.username);
        }
        else {
          users[k].userstatus = OFFLINE;
          Print_user(wnd_users, k, rmsg.username);
        }
        break;

      default:
        break;
    }
  }
  if (status == -1)
    err_exit_pth;

  pthread_cleanup_pop(1);
  pthread_cleanup_pop(1);
  pthread_cleanup_pop(1);
}

/* Function of reader message queue for thread */
void* Thread_sender(void* arg) {
  int msqCS;                /* Client -> Server */
  key_t keyCS;
  int status;
  struct msgbuf smsgbuf;
  struct Message smsg;
  WINDOW *wnd_newmsg;       /* Window for typing new text message */
  char message[TEXT_SIZE];  /* New text message */
  int k;
  int ch;
  int xmax, ymax;

  /* Memory allocation and set thread clean-up hadlers */
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL); 
  pthread_cleanup_push(Unlock_sem, &sem);
  
  wnd_newmsg = newwin(HEIGTH_INPUT, COLS, LINES - HEIGTH_INPUT, 0);
  pthread_cleanup_push(Delete_window, wnd_newmsg);
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  
  keypad(wnd_newmsg, TRUE);

  keyCS = ftok(BINNAME, KEYCS);
    if (keyCS == -1)
      err_exit_pth;
  msqCS = msgget(keyCS, 0);
    if (msqCS == -1)
      err_exit_pth;
 
  getmaxyx(wnd_newmsg, ymax, xmax);
  smsg.spid = getpid();
  strncpy(smsg.username, (char *) arg, USERNAME_SIZE);

  int flag = 0;
  while(1) {
    /* Typing a new text message */
    k = 0;
    int x = 0, y = 0; /* current position */
    while (k < TEXT_SIZE - 1) {
      ch = wgetch(wnd_newmsg);
      if (ch == KEY_F(1)) {
        flag = 1;
        break;
      }
      else if (ch == '\n')
        break;
      if (ch == KEY_BACKSPACE) {
        if (y == 0 & x == 0)
          continue;
        else if (x == 0) {
          y--;
          x = xmax - 1;
        }
        else
          x--;
        mvwaddch(wnd_newmsg, y, x, ' ');
        wrefresh(wnd_newmsg);
        wmove(wnd_newmsg, y, x);
        k--;
        continue;
      }
      if (x == xmax - 1) {
        x = 0;
        y++;
      }
      else
        x++;
      wechochar(wnd_newmsg, ch);
      message[k++] = ch;
    }
    werase(wnd_newmsg);
    if (flag == 1)
      break;

    /* Sending new text message on server */
    message[k] = '\0';     
    smsg.type = TEXT;
    strncpy(smsg.text, message, TEXT_SIZE); 
    status = Send(msqCS, &smsg, sizeof(struct Message), 0, PR_TEXT);
    if (status == -1)
      err_exit_pth;
  }
 
  /* Sending exit notification on server */ 
  smsg.type = EXIT;
  status = Send(msqCS, &smsg, sizeof(struct Message), 0, PR_EXIT);
  if (status == -1)
    err_exit_pth;
  pthread_cleanup_pop(1);
  pthread_cleanup_pop(1);
}

/* For atexit() destroying semaphor */
void Destroy_sem(void) {
  if (sem_destroy(&sem) == -1)
    perror("sem_destroy");
}


int main (void) {
  char username[USERNAME_SIZE];   /* Username of current user */
  int msqSC;                      /* Server -> Client */
  int msqCS;                      /* Client -> Server */
  key_t keySC, keyCS;
  int status;
  struct msgbuf rmsgbuf;
  struct Message rmsg, smsg;
  pid_t pid;                      /* Pid of current program */
  pthread_t th_reader, th_sender;

  pid = getpid();
  
  keySC = ftok(BINNAME, KEYSC);
    if (keySC == -1)
      err_exit("ftok");
  keyCS = ftok(BINNAME, KEYCS);
    if (keyCS == -1)
      err_exit("ftok");

  msqSC = msgget(keySC, 0);
    if (msqSC == -1)
      err_exit("msgget");
  msqCS = msgget(keyCS, 0);
    if (msqCS == -1)
      err_exit("msgget");

  /* Registration attempt */
  printf("Enter nickname:\n");
  fgets(username, USERNAME_SIZE, stdin);
    if (strlen(username) > 0 && username[strlen(username) - 1] == '\n')
    username[strlen(username) - 1] = '\0';

  /* Sending registration request */
  smsg.type = REGISTRATION_REQUEST;
  smsg.spid = pid;
  strncpy(smsg.username, username, USERNAME_SIZE);
  status == Send(msqCS, &smsg, sizeof(struct Message), 0, PR_RREQ);
  if (status == -1)
      err_exit("msgsnd");

  /* Response processing */
  status = msgrcv(msqSC, &rmsgbuf, sizeof(struct Message), pid, 0);
  if (status == -1)
    err_exit("msgrcv");
  rmsg = rmsgbuf.data;
  switch (rmsg.type) {
    case REGISTRATION_ACCEPTED:
      printf("Registration accepted\n");
      break;
    
    case REGISTRATION_REJECTED:
      printf("Registration rejected: %s", rmsg.text);
      exit(EXIT_SUCCESS);
      break;

    default:
      printf("Unrecognized response\n");
      exit(EXIT_SUCCESS);
      break;
  }

  initscr();
  curs_set(TRUE);
  noecho();
  
  /* Checking the size of the terminal */
  if (COLS < MIN_COLS || LINES < MIN_LINES) {
    endwin();
    fprintf(stderr, "Terminal size must be at least %d cols and %d rows\n",
           MIN_COLS, MIN_LINES);
    exit(EXIT_FAILURE);
  }

  /* Drawing window dividers */
  for (int i = 0; i < COLS; i++)
    mvaddch(LINES - 1 - HEIGTH_INPUT, i, '-');
  for (int i = 0; i < LINES - 1 - HEIGTH_INPUT; i++)
    mvaddch(i, COLS - 1 - WIDTH_USERS, '|');
  refresh();
  
  /* Launching the client */  
  status = sem_init(&sem, 0, 0);
  if (status == -1) {
    endwin();
    err_exit("sem_init");
  }
  status == atexit(Destroy_sem);
  if (status != 0)
    err_exit2("atexit");
  status = pthread_create(&th_reader, NULL, Thread_reader, NULL);
  if (status != 0)
    err_exit2("Error: pthread_create");
  status = pthread_create(&th_sender, NULL, Thread_sender, username);
  if (status != 0)
    err_exit2("Error: pthread_create");
 
  /* Waiting for the completion of any of the threads */ 
  status = sem_wait(&sem);
  if (status == -1)
    err_exit("sem_wait");
  
  /* Cancel the remaining thread */
  status = pthread_cancel(th_reader);
  if (status != 0)
    err_exit2("Error: pthread_cancel");
  status = pthread_cancel(th_sender);
  if (status != 0)
    err_exit2("Error: pthread_cancel");
  status = pthread_join(th_reader, NULL);
  if (status != 0)
    err_exit2("Error: pthread_join");
  status = pthread_join(th_sender, NULL);
  if (status != 0)
    err_exit2("Error: pthread_join");

  endwin();
  exit(EXIT_SUCCESS);
}
