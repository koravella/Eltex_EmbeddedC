#include "display.h"
#include <stdlib.h>
#include <math.h>

/* Rounds up x / y (x and y must be positive integer) */
#define ceill(x, y) ((x + y - 1) / y)

struct Display* New_display() {
  struct Display *dsp;

  dsp = (struct Display*) malloc(sizeof(struct Display));
  dsp->x = dsp->y = 0;
  dsp->first = dsp->last = 0;
  dsp->buf = New_buffer();
  dsp->wnd = newwin(LINES - INFOSIZE, COLS, 0, 0);
  return dsp;
}

void Delete_display(struct Display* dsp) {
  Delete_buffer(dsp->buf);
  delwin(dsp->wnd);
  free(dsp);
}

void Upload_from_file(struct Display *dsp, int fd) {
  Read_file(dsp->buf, fd);
}

void Save_to_file(struct Display *dsp, int fd) {
  Write_file(dsp->buf, fd);
}

int Fit_down(struct Display *dsp, int nl) {
  int ret;
  int sum, ymax, xmax;

  getmaxyx(dsp->wnd, ymax, xmax);
  sum = 0;
  for (ret = 0; sum < ymax && nl + ret < dsp->buf->nlines; ret++) {
    int sz = dsp->buf->line[nl + ret]->size;
    /* Discarding newline */
    if (sz > 1)
      sz--;
    sum += ceill(sz, xmax);
  }
  return ret - 1;
}

int Fit_up(struct Display *dsp, int nl) {
  int ret;
  int sum, ymax, xmax;

  getmaxyx(dsp->wnd, ymax, xmax);
  sum = 0;
  for (ret = 0; sum <= ymax && nl - ret >= 0; ret++) {
    int sz = dsp->buf->line[nl - ret]->size;
    /* Discarding newline */
    if (sz > 1)
      sz--; 
    sum += ceill(sz, xmax);
  }
  return ret - 1;
}

void Display_text(struct Display *dsp, int first) {
  int fit, prt_size;
  int xmax, ymax;
  int sum, ret;

  getmaxyx(dsp->wnd, ymax, xmax);
  fit = Fit_down(dsp, first);
  werase(dsp->wnd);
  for (int i = first; i <= first + fit; i++) {
    prt_size = dsp->buf->line[i]->size;
    /* Discarding newline when it is superfluous */
    if (prt_size % xmax == 1 && prt_size > xmax)
      prt_size--;
    waddnstr(dsp->wnd, dsp->buf->line[i]->symb, prt_size);
  }
  
  dsp->first = first;
  dsp->last = first + fit;

  /* Returns the cursor to the displayed area */
  if (dsp->y < dsp->first || dsp->y > dsp->last) {
    dsp->y = dsp->first;
    dsp->x = 0;
  }
  
  wrefresh(dsp->wnd);
  Set_cursor(dsp);
}

void Set_cursor(struct Display *dsp) {
  int ydisp, xdisp, xmax, ymax;

  curs_set(TRUE);
  getmaxyx(dsp->wnd, ymax, xmax);
  
  ydisp = 0;
  for (int i = dsp->first; i < dsp->y; i++) {
    int sz = dsp->buf->line[i]->size;
    /* Discarding newline */
    if (sz > 1)
      sz--; 
    ydisp += ceill(sz, xmax);
  }
  ydisp += dsp->x / xmax;
  xdisp = dsp->x % xmax;
  
  wmove(dsp->wnd, ydisp, xdisp);
  wrefresh(dsp->wnd);
}

void Cursor_left(struct Display *dsp) {
  if (dsp->x > 0)
    dsp->x--;
  Set_cursor(dsp);
}

void Cursor_right(struct Display *dsp) {
  if (dsp->x < dsp->buf->line[dsp->y]->size - 2)
    dsp->x++;
  Set_cursor(dsp);
}

void Cursor_up(struct Display *dsp) {
  int sz_new;

  if (dsp->y == 0)
    return;
  
  dsp->y--;
  sz_new = dsp->buf->line[dsp->y]->size;
  if (dsp->x > sz_new - 2)
    dsp->x = sz_new - 2 >= 0 ? sz_new - 2 : 0;

  if (dsp->y < dsp->first)
    Display_text(dsp, dsp->first - 1);
}

void Cursor_down(struct Display *dsp) {
  int sz_new;

  if (dsp->y == dsp->buf->nlines - 1)
    return;

  dsp->y++;
  sz_new = dsp->buf->line[dsp->y]->size;
  if (dsp->x > sz_new - 2)
    dsp->x = sz_new - 2 >= 0 ? sz_new - 2 : 0;

  if (dsp->y > dsp->last)
    Display_text(dsp, dsp->first + 1);
}

int Replace(struct Display *dsp, char ch) {
  /*
   * Does not allow replacing new line (the cursor appears on it if the entire
   * line consists only of newline)
   */
  if (dsp->buf->line[dsp->y]->symb[dsp->x] != '\n') {
    dsp->buf->line[dsp->y]->symb[dsp->x] = ch;
    Set_cursor(dsp);
    waddch(dsp->wnd, ch);
    Cursor_right(dsp); 
    return 1;
  }
  return 0;
}
