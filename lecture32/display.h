/*
 * The structure for managing the contents of an open directory displayed on the
 * screen.
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <curses.h>

/*
 * The auxiliary 'enum Cursor' makes it clear if the current Display has
 * content and if so, where the current position is located: among
 * directories or among files.
 */
enum Cursor {
  NOCONTENT,
  DIRS,
  FILES
};

/*
 * The 'wnd' field contains ncurses window for box and 'subwnd' field -- the one
 * on which the current content of directory, stored in 'dirs' and 'files' of
 * lenght 'size_dirs' and 'size_files' respectively, is displayed. The current
 * position is saved using 'cur' and 'line', and the 'hl_line' line of 'subwnd'
 * corresponds to this position.
 */
struct Display {
  WINDOW *wnd;
  WINDOW *subwnd;
  struct dirent **dirs;
  struct dirent **files;
  int size_dirs, size_files;
  enum Cursor cur;
  int line;
  int hl_line;
};

struct Display* New_display(int lines, int cols, int ypos, int xpos);
void Delete_display(struct Display* dsp);

/*
 * Reads in 'dsp' the contents of the directory on path 'dirpath'. Current
 * position is set to the beggining.
 */
int Read_content(struct Display *dsp, char *dirpath);

/*
 * Returns the name of the current position in 'dsp'.
 */
char* Select(struct Display *dsp);

/*
 * Displays the contents of the 'dsp' on the screen, setting the current
 * highlighted position to the beginning.
 */
int Show_display(struct Display *dsp);

/*
 * Sets and removes the highlight of the current position.
 */
void Set_highlight(struct Display *dsp);
void Reset_highlight(struct Display *dsp);

/*
 * Moves the current position up and down respectively. Scroll the displayed
 * text if necessary.
 */ 
void Up(struct Display *dsp);
void Down(struct Display *dsp);

#endif
