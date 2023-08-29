#include <sys/ioctl.h>
#include <signal.h>
#include <curses.h>
#include "display.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>

#define err_exit(msg)   do {endwin(); perror(msg); exit(EXIT_FAILURE);} while(0)

#define INFOOFFSET 1 /* Bottom indentation for infopanel */

/* 
 * Avoids termination with an error when resizing the terminal.
 */
void Sig_winch(int signo) {
  struct winsize size;

  ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
  resizeterm(size.ws_row, size.ws_col);
}

/*
 * To indicate the current working display.
 */
enum Current_display {
  DSP1,
  DSP2
};

int main(void) {
  struct Display *dsp1, *dsp2, *curdsp;
  int cmd;
  char *nextdir;
  char *cmd_str;
  char *curpath1, *curpath2, *curpath, *oldpath;

  initscr();
  signal(SIGWINCH, Sig_winch);
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(FALSE);

  cmd_str = "TAB: change display  F1: exit";
  attron(A_STANDOUT);
  mvaddstr(LINES - 1, 0, cmd_str);
  attroff(A_STANDOUT);
  refresh();

  dsp1 = New_display(LINES - INFOOFFSET, (COLS - 1) / 2, 0, 0);
  dsp2 = New_display(LINES - INFOOFFSET, (COLS - 1) / 2, 0, COLS - (COLS-1)/2);
  if (Read_content(dsp1, ".") == -1)
    err_exit("Read_content");
  if (Read_content(dsp2, ".") == -1)
    err_exit("Read_content");
  Show_display(dsp1);
  Show_display(dsp2);
  Reset_highlight(dsp2);
  enum Current_display cur = DSP1;
  curdsp = dsp1;
  curpath1 = getcwd(NULL, 0);
  curpath2 = getcwd(NULL, 0);
  curpath = curpath1;

  while ((cmd = getch()) != KEY_F(1)) {
    switch (cmd) {
      case '\t':
        Reset_highlight(curdsp);
        if (cur == DSP1) {
          cur = DSP2;
          curdsp = dsp2;
          curpath1 = curpath;
          curpath = curpath2;
        }
        else if (cur == DSP2) {
          cur = DSP1;
          curdsp = dsp1;
          curpath2 = curpath;
          curpath = curpath1;
        }
        Set_highlight(curdsp);
        break;
      
      case '\n':
        oldpath = strdup(curpath);
        nextdir = Select(curdsp);
        if (strcmp(nextdir, "..") == 0) {
          int k;
         
          /* A simple analogue of basename() */
          if (strlen(curpath) != 1) { 
            for (k = strlen(curpath) - 1; k >= 0; k--)
              if (curpath[k] == '/')
                break;
            if (k != 0) {
              curpath = realloc(curpath, k + 1);
              curpath[k] = '\0';
            }
            else {
              curpath = realloc(curpath, k + 2);
              curpath[k + 1] = '\0';
            }
          }
        }
        else {
          curpath = realloc(curpath, strlen(curpath) + strlen(nextdir) + 2);
          strcat(curpath, "/");
          strcat(curpath, nextdir);
        }
        
        /* Try to move along the specified path and if impossible, return */
        if (Read_content(curdsp, curpath) == -1) {
          free(curpath);
          curpath = oldpath;
          if (Read_content(curdsp, curpath) == -1)
            err_exit("Read_content");
        }
        else
          free(oldpath);
        Show_display(curdsp);
        break;
      
      case KEY_UP:
        Up(curdsp);
        break;
      
      case KEY_DOWN:
        Down(curdsp);
        break;

      default:
        break;
    }
  }
  
  if (cur == DSP1)
    curpath1 = curpath;
  else
    curpath2 = curpath;
  free(curpath1);
  free(curpath2);
  Delete_display(dsp1);
  Delete_display(dsp2);
  endwin();
  return EXIT_SUCCESS;
}
