/*
 * The structure for managing the contents of an open file displayed on the
 * screen. It works correctly only if there are no unprintable characters and
 * tabs in the file.
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "buffer.h"
#include <curses.h>

#define INFOSIZE 4 /* The size of offset for the infopanel at the bottom */

/*
 * The 'buf' field contains the entire text. The 'wnd' field contains ncurses
 * window on which the current text is displayed. Here 'first' and 'last',
 * respectively, the first and last line of the displayed text. Also the 'x'
 * and 'y' position where the cursor is set (string and character) in the whole
 * text.
 */
struct Display {
  int x, y;
  int first, last;
  struct Buffer* buf;
  WINDOW *wnd;
};

struct Display* New_display();
void Delete_display(struct Display* dsp);

/* Here 'fd' is a file descriptor */
void Upload_from_file(struct Display* dsp, int fd);
void Save_to_file(struct Display* dsp, int fd);

/*
 * Calculates how many lines of the entire text may be displayed on the current
 * 'dsp', starting from the line 'nl' down and up respectively.
 */
int Fit_down(struct Display *dsp, int nl);
int Fit_up(struct Display *dsp, int nl);

/*
 * Displays the maximum possible number of lines of text, starting with the
 * line 'first'.
 */
void Display_text(struct Display *dsp, int first);

/*
 * Sets the cursor to the correct relative position on the screen, calculated
 * by absolute position 'dsp->x' and 'dsp->y'.
 */
void Set_cursor(struct Display *dsp);

/*
 * Moves the cursor left and right respectively.
 */ 
void Cursor_left(struct Display *dsp);
void Cursor_right(struct Display *dsp);

/*
 * Moves the cursor up and down respectively. Scroll the displayed text if
 * necessary.
 */ 
void Cursor_up(struct Display *dsp);
void Cursor_down(struct Display *dsp);

/*
 * Replaces the current character pointed to by the cursor with 'ch', if
 * necessary. Returns 1 if the replacement is successful and 0 otherwise.
 */
int Replace(struct Display *dsp, char ch);

#endif
