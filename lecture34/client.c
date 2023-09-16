/*
 * Client app for exchanging messages via shared memory created by server
 */

#include "declarations.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

/* Minimal required size of terminal */
#define MIN_COLS 80
#define MIN_LINES 25

#define HEIGTH_INFOPANEL 1  /* Size of info panel in bottom of the screen */
#define HEIGTH_INPUT 4      /* Size of input window */
#define WIDTH_USERS 19      /* Size of the user display window */

/* Size of bottom panels */
#define HEIGTH_BOT (HEIGTH_INFOPANEL + HEIGTH_INPUT)

/* Macroses for early termination */
/* 1: without semaphore post */
#define err_exit(msg)   do {endwin();\
                            perror(msg);\
                            exit(EXIT_FAILURE);} while(0)
/* 2: with semaphore post */
#define exit_free(code, sem)    do {sem_post(sem);\
                                    endwin();\
                                    exit(code);} while(0)

/* Integer rounding up x/y */
#define ceill(x, y) ((x + y - 1) / y)

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

/* 
 * Show new messages and changing displayed list of users
 * Return -1 if server stopped, -2 if chat is full and 0 otherwise
 */
int Update_chat(WINDOW* wnd_msgs,
                WINDOW* wnd_users,
                struct Bufmsg *shmbuf,
                sem_t *sem_access,
                int *num_disp_msg) {
  int xmax_msgs, ymax_msgs;
  struct Message msg;
  int nlines;
  int k;
  int ret;    /* Value for return */
  int flag;   /* 1 -- server was stopped, 0 -- otherwise */

  getmaxyx(wnd_msgs, ymax_msgs, xmax_msgs);
  
  if (sem_wait(sem_access) == -1)
    err_exit("sem_wait");

  ret = 0;
  flag = 0;
  for (int i = *num_disp_msg; i < shmbuf->num_msgs; i++) {
    msg = shmbuf->msgs[i];

    switch(msg.type) {
      
      /* Processing a new text message */
      case TEXT:
        nlines = ceill(USERNAME_SIZE + strlen(msg.text), xmax_msgs); 
        if (nlines > ymax_msgs)
          nlines = ymax_msgs;
        wscrl(wnd_msgs, nlines);
        mvwprintw(wnd_msgs, ymax_msgs - nlines, 0, "%s: %s",
                  msg.username, msg.text);
        wrefresh(wnd_msgs);
        break;

      /* Processing a notification about online user */
      case USER_ONLINE:
        if ((k = Registred(shmbuf->users, MAX_USERS, msg.username)) != -1)
          Print_hightlight_user(wnd_users, k, msg.username);
        break;
      
      /* Processing a notification about offline user */
      case USER_OFFLINE:
        if ((k = Registred(shmbuf->users, MAX_USERS, msg.username)) != -1)
          Print_user(wnd_users, k, msg.username);
        break;
      
      /* Processing a notification about stopped server */
      case STOP:
        flag = 1;
        break;

      default:
        break;
    }
  }
  *num_disp_msg = shmbuf->num_msgs;

  if (flag == 1)  /* Server was stopped */
    ret = -1;
  if (shmbuf->num_msgs == MAX_MESSAGES)
    ret = -2;
  
  if (sem_post(sem_access) == -1)
    err_exit("sem_post");
  return ret;
}

/* 
 * Process all user input.
 * A new message will be written to 'message' for sending.
 * Return 0 -- on normal, -1 on server stopped, -2 on overfull of chat, 
 * -3 on exit command from user.
 */
int Enter_message(char message[],
                  WINDOW* wnd_newmsg,
                  WINDOW* wnd_msgs,
                  WINDOW* wnd_users,
                  struct Bufmsg *shmbuf,
                  sem_t *sem_access,
                  int *num_disp_msg) {
  int xmax, ymax;
  int k;
  int ch;
  int ret; /* Value for return */
 
  getmaxyx(wnd_newmsg, ymax, xmax);

  /* Typing a new text message */
  k = 0;
  int x = 0, y = 0; /* current position */
  ret = 0;
  while (k < TEXT_SIZE - 1) {
    ch = wgetch(wnd_newmsg);
    
    if (ch == KEY_F(1)) {
      ret = -3;
      break;
    }
    else if (ch == KEY_F(2)) {
      ret = Update_chat(wnd_msgs, wnd_users, shmbuf, sem_access, num_disp_msg);
      if (ret != 0)
        break;
      continue;
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
  message[k] = '\0';
 
  werase(wnd_newmsg);
  return ret;
}

/* 
 * Send new message with with the specified fields.
 * Return -1 if chat is overfull and 0 otherwise
 */
int Send(struct Bufmsg *shmbuf, enum Type type, char username[], char text[]) {
  if (shmbuf->num_msgs == MAX_MESSAGES)
    return -1;

  shmbuf->msgs[shmbuf->num_msgs].type = type;
  strncpy(shmbuf->msgs[shmbuf->num_msgs].username, username, USERNAME_SIZE);
  strncpy(shmbuf->msgs[shmbuf->num_msgs].text, text, TEXT_SIZE);
  shmbuf->num_msgs++;
  return 0;
}

/* Entering username and registration attempt */
void Registration(struct Bufmsg *shmbuf, sem_t *sem_access, char username[]) {
  int k;

  /* Entering a username */
  printf("Enter nickname:\n");
  fgets(username, USERNAME_SIZE, stdin);
  if (strlen(username) > 0 && username[strlen(username) - 1] == '\n')
    username[strlen(username) - 1] = '\0';
  
  /* Registration attempt */
  if (sem_wait(sem_access) == -1)
    err_exit("sem_access");
  
  if (shmbuf->num_msgs >= MAX_MESSAGES) {
    printf("Too many messages in chat\n");
    exit_free(EXIT_SUCCESS, sem_access);
  }
  
  if ((k = Registred(shmbuf->users, MAX_USERS, username)) == -1) {
    /* New username */
    if (shmbuf->num_users < MAX_USERS) {
      strncpy(shmbuf->users[shmbuf->num_users].username,username,USERNAME_SIZE);
      shmbuf->users[shmbuf->num_users].userstatus = ONLINE;
      shmbuf->num_users++;
      Send(shmbuf, USER_ONLINE, username, "");
    }
    else {
      printf("Too many users in cnat\n");
      exit_free(EXIT_SUCCESS, sem_access);
    }
  }
  else {
    /* Such username is already exist */
    if (shmbuf->users[k].userstatus == OFFLINE) {
      shmbuf->users[k].userstatus = ONLINE;
      Send(shmbuf, USER_ONLINE, username, "");
    }
    else {
      printf("User with this username is already online\n");
      exit_free(EXIT_SUCCESS, sem_access);
    }
  }
  
  if (sem_post(sem_access) == -1)
    err_exit("sem_post");
}

/* Checking the screen size and initial drawing in ncurses */
void Start_display() {
  /* Checking the size of the terminal */
  if (COLS < MIN_COLS || LINES < MIN_LINES) {
    endwin();
    fprintf(stderr, "Terminal size must be at least %d cols and %d rows\n",
           MIN_COLS, MIN_LINES);
    exit(EXIT_FAILURE);
  }

  /* Drawing window dividers */
  for (int i = 0; i < COLS; i++)
    mvaddch(LINES - 1 - HEIGTH_BOT, i, '-');
  for (int i = 0; i < LINES - 1 - HEIGTH_BOT; i++)
    mvaddch(i, COLS - 1 - WIDTH_USERS, '|');
  refresh();

  /* Printing infopanel */
  attron(A_REVERSE);
  mvaddstr(LINES - 1, 0, "F1: exit  F2: update messages");
  attroff(A_REVERSE);
  refresh();
}

int main (void) {
  char username[USERNAME_SIZE];   /* Username of current user */
  int status;
  sem_t *sem_access;
  int fd;
  struct Bufmsg *shmbuf;
  int k;
  WINDOW *wnd_newmsg;             /* Window for typing new text message */
  WINDOW *wnd_msgs;               /* Window for displaying text messages */
  WINDOW *wnd_users;              /* Window for displaying registred users */
  int num_disp_msg;               /* Current number of displayed messages */
  char message[TEXT_SIZE];  /* New text message */

  /* Opening named semaphore and shared memory created by server */
  sem_access = sem_open(SEMNAME, 0);
  if (sem_access == SEM_FAILED)
    err_exit("sem_open");
  fd = shm_open(SHMNAME, O_RDWR, 0);
  if (fd == -1)
    err_exit("shm_open");
  shmbuf = mmap(NULL, sizeof(struct Bufmsg), PROT_READ | PROT_WRITE,
                MAP_SHARED, fd, 0);
  if (shmbuf == (void *) -1)
    err_exit("mmap");

  Registration(shmbuf, sem_access, username);
  
  initscr();
  curs_set(TRUE);
  noecho();
  
  Start_display();

  /*
   * Creating windows for displaying messages, users and the
   * message being entered.
   */
  wnd_newmsg = newwin(HEIGTH_INPUT, COLS, LINES - HEIGTH_BOT, 0);
  keypad(wnd_newmsg, TRUE);

  wnd_msgs = newwin(LINES - 1 - HEIGTH_BOT, COLS - 1 - WIDTH_USERS, 0, 0);
  wnd_users = newwin(LINES - 1 - HEIGTH_BOT,WIDTH_USERS, 0, COLS - WIDTH_USERS); 
  scrollok(wnd_msgs, TRUE);
  werase(wnd_msgs);
  wrefresh(wnd_msgs);
  
  /* status: -3 -- exit, -2 -- chat overfull, -1 -- server stopped, 0--normal */
  /* Showing existing messages */
  num_disp_msg = 0;
  status = Update_chat(wnd_msgs, wnd_users, shmbuf, sem_access, &num_disp_msg); 
  if (status == 0) {
    /* Processing user input */  
    while ((status = Enter_message(message, wnd_newmsg, wnd_msgs, wnd_users,
                                  shmbuf, sem_access, &num_disp_msg)) == 0) {
      if (sem_wait(sem_access) == -1)
        err_exit("sem_wait");
      
      if (Send(shmbuf, TEXT, username, message) == -1) {
        status = -2;
      }
      
      if (sem_post(sem_access) == -1)
        err_exit("sem_post");
      if (status == 0) 
        status = Update_chat(wnd_msgs, wnd_users, shmbuf,
                             sem_access, &num_disp_msg); 
    }
    
    /* The user exits */
    if (status == -3) {
      if (sem_wait(sem_access) == -1)
        err_exit("sem_wait");
      if ((k = Registred(shmbuf->users, MAX_USERS, username)) != -1)
        shmbuf->users[k].userstatus = OFFLINE;
      if (Send(shmbuf, USER_OFFLINE, username, message) == -1)
        status = -2;
      if (sem_post(sem_access) == -1)
        err_exit("sem_post");
    }
  }

  delwin(wnd_msgs);
  delwin(wnd_newmsg);
  delwin(wnd_users);
  endwin(); 
  
  if (status == -1) 
    printf("Server stopped\n");
  else if (status == -2)
    printf("Too many messages in chat\n");
  
  if (munmap(shmbuf, sizeof(struct Bufmsg)) == -1)
    err_exit("munmap");
  if (sem_close(sem_access) == -1)
    err_exit("sem_close");
  exit(EXIT_SUCCESS);
}
