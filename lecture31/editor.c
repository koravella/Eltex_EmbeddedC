#include <sys/ioctl.h>
#include <signal.h>
#include <curses.h>
#include "buffer.h"
#include "display.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_LEN_PATH 1024 /* Maximum buffer length for a pathname */

/* Status of the currently open file */
#define SAVED 0
#define CHANGED 1

/* Editor status regarding file opening */
#define NOTOPEN 0
#define OPEN 1

/* 
 * Avoids termination with an error when resizing the terminal.
 */
void Sig_winch(int signo) {
  struct winsize size;

  ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
  resizeterm(size.ws_row, size.ws_col);
}

void Start_editor(void) {
  char *cmd_str;

  initscr();
  signal(SIGWINCH, Sig_winch);
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(FALSE);
  cmd_str = "F1: open  F2: save  F3: exit";
  move(LINES - 4, 0);
  for (int i = 0; i < COLS; i++)
    addch('-');
  attron(A_REVERSE);
  mvaddstr(LINES - 1, 0, cmd_str);
  attroff(A_REVERSE);
  refresh();
}

void End_editor() {
  endwin();
}

/*
 * Displays a window for entering the file path.
 */
void Get_path(char input[]) {
  WINDOW *wnd, *subwnd;;

  wnd = newwin((LINES - INFOSIZE) / 2, COLS / 2,
               (LINES - INFOSIZE) / INFOSIZE, COLS / INFOSIZE);
  box(wnd, '|', '-');
  
  subwnd = derwin(wnd, (LINES - INFOSIZE) / 2 - 2, COLS / 2 - 2, 1, 1);
  wattron(subwnd, A_BOLD);
  wprintw(subwnd, "Enter a pathname:\n");
  wattroff(subwnd, A_BOLD);
  curs_set(TRUE);
  echo();
  wrefresh(wnd);
  wgetnstr(subwnd, input, MAX_LEN_PATH);
  input[MAX_LEN_PATH] = 0;
  
  delwin(subwnd);
  delwin(wnd);
  curs_set(FALSE);
  noecho();
}

/*
 * Displays an error window for an error path.
 */
void Err_path() {
  WINDOW *wnd, *subwnd;

  curs_set(FALSE);
  wnd = newwin((LINES - INFOSIZE) / 2, COLS / 2,
               (LINES - INFOSIZE) / INFOSIZE, COLS / INFOSIZE);
  box(wnd, '|', '-');
  
  subwnd = derwin(wnd, (LINES - INFOSIZE) / 2 - 2, COLS / 2 - 2, 1, 1);
  wattron(subwnd, A_STANDOUT);
  wprintw(subwnd, "Incorrect pathname\nPress 'q' to exit");
  wattroff(subwnd, A_STANDOUT);
  wrefresh(wnd);
  
  delwin(subwnd);
  delwin(wnd);
  while (getch() != 'q');
}

/*
 * Displays the current cursor position in the text.
 */
void Print_pos(struct Display *dsp) {
  curs_set(FALSE);
  move(LINES - 3, 0);
  clrtoeol();
  mvprintw(LINES - 3, 0, "line: %d, col: %d", dsp->y + 1, dsp->x + 1);
  refresh();
}

/*
 * Displays the status of an open file.
 */
void Print_status(int st) {
  curs_set(FALSE);
  move(LINES - 2, 0);
  clrtoeol();
  if (st == SAVED)
    mvaddstr(LINES - 2, 0, "Saved");
  else if (st == CHANGED)
    mvaddstr(LINES - 2, 0, "Not saved");
  refresh();
}

int main(void) {
  Start_editor();

  struct Display* dsp;
  int fd;
  int cmd;
  char input[MAX_LEN_PATH];
  int st;

  dsp = New_display();
  st = NOTOPEN;
  while((cmd = getch()) != KEY_F(3)) {
    if (cmd == KEY_F(1)) {
      Get_path(input);
      fd = open(input, O_RDONLY|O_CREAT, 00666);
      if (fd == -1) {
        Err_path();
        Display_text(dsp, 0);
        curs_set(FALSE);
      }
      else {
        Delete_display(dsp);
        dsp = New_display();
        Upload_from_file(dsp, fd);
        st = OPEN;
        close(fd);
         
        Display_text(dsp, 0);
        Print_pos(dsp);
        Print_status(SAVED);
        Set_cursor(dsp);
      }
    }
    else if (st == OPEN) {
      switch (cmd) { 
        case KEY_F(2):
          if (st == NOTOPEN)
            break;
          fd = open(input, O_WRONLY|O_CREAT, 00666);
          if (fd == -1) {
            Delete_display(dsp);
            End_editor();
            fprintf(stderr, "Error: open on write\n");
            exit(EXIT_FAILURE);
          }
          Save_to_file(dsp, fd);
          close(fd);
          
          Print_status(SAVED);
          Set_cursor(dsp);
          break;
      
        case ERR:
          Delete_display(dsp);
          End_editor();
          fprintf(stderr, "Error: getch\n");
          exit(EXIT_FAILURE);
          break;
      
        case KEY_LEFT:
          Cursor_left(dsp);
          Print_pos(dsp);
          Set_cursor(dsp);
          break;
      
        case KEY_RIGHT:
          Cursor_right(dsp);
          Print_pos(dsp);
          Set_cursor(dsp);
          break;
      
        case KEY_UP:
          Cursor_up(dsp);
          Print_pos(dsp);
          Set_cursor(dsp);
          break;
      
        case KEY_DOWN:
          Cursor_down(dsp);
          Print_pos(dsp);
          Set_cursor(dsp);
          break;
        
        default:
          if (Replace(dsp, (char) cmd))
            Print_status(CHANGED);
          Print_pos(dsp);
          Set_cursor(dsp);
          break;
      }
    }
  }

  Delete_display(dsp);
  End_editor();
  return EXIT_SUCCESS;
}
