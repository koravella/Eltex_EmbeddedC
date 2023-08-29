#include "display.h"
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

struct Display* New_display(int lines, int cols, int ypos, int xpos) {
  struct Display *dsp;

  dsp = (struct Display*) malloc(sizeof(struct Display));
  dsp->wnd = newwin(lines, cols, ypos, xpos);
  dsp->subwnd = derwin(dsp->wnd, lines - 2, cols - 2, 1, 1);
  dsp->dirs = dsp->files = NULL; 
  dsp->cur = NOCONTENT;
  scrollok(dsp->subwnd, TRUE);

  box(dsp->wnd, '|', '-');
  wrefresh(dsp->wnd);
  return dsp;
}

void Delete_display(struct Display* dsp) {
  delwin(dsp->subwnd);
  delwin(dsp->wnd);
  if (dsp->dirs != NULL) {
    for (int i = 0; i < dsp->size_dirs; i++)
      free(dsp->dirs[i]);
    free(dsp->dirs);
  }
  if (dsp->files != NULL) {
    for (int i = 0; i < dsp->size_files; i++)
      free(dsp->files[i]);
    free(dsp->files);
  }
  free(dsp);
}

/*
 * Returns 1 if 'ent' is a directory and 0 otherwise.
 */
int filter_dir(const struct dirent *ent) {
  if (ent->d_type == DT_DIR && strcmp(ent->d_name, ".") != 0)
    return 1;
  return 0;
}

/*
 * Returns 1 if 'ent' is not a directory and 0 otherwise.
 */
int filter_notdir(const struct dirent *ent) {
  if (ent->d_type != DT_DIR)
    return 1;
  return 0;
}

int Read_content(struct Display *dsp, char* dirpath) {
  /* If the arrays were not empty, then we will clear them */
  if (dsp->dirs != NULL) {
    for (int i = 0; i < dsp->size_dirs; i++)
      free(dsp->dirs[i]);
    free(dsp->dirs);
  }
  if (dsp->files != NULL) {
    for (int i = 0; i < dsp->size_files; i++)
      free(dsp->files[i]);
    free(dsp->files);
  }
  dsp->dirs = dsp->files = NULL;
  dsp->cur = NOCONTENT;

  dsp->size_dirs = scandir(dirpath, &dsp->dirs, filter_dir, alphasort);
  if (dsp->dirs == NULL)
    return -1;
  dsp->size_files = scandir(dirpath, &dsp->files, filter_notdir, alphasort);
  if (dsp->files == NULL)
    dsp->size_files = 0;
  dsp->cur = DIRS;
  dsp->line = 0;
  dsp->hl_line = 0;
  return dsp->size_dirs + dsp->size_files;
}

char* Select(struct Display *dsp) {
  if (dsp->cur == DIRS)
    return dsp->dirs[dsp->line]->d_name;
  else if (dsp->cur == FILES)
    return dsp->files[dsp->line]->d_name;
  /* dsp->cur == NOCONTENT */
  return NULL;
}

/*
 * Outputs in the 'wndline' line of 'dsp' the contents at the position
 * specified by 'pcur' and 'pline'.
 */
void Print(struct Display *dsp, enum Cursor pcur, int pline, int wndline) {
  int ymax, xmax;
  
  if (pcur == NOCONTENT)
    return;

  getmaxyx(dsp->subwnd, ymax, xmax);
  wmove(dsp->subwnd, wndline, 0);
  if (pcur == DIRS) {
    waddch(dsp->subwnd, '/');
    if (strlen(dsp->dirs[pline]->d_name) > xmax - 1) {
      waddnstr(dsp->subwnd, dsp->dirs[pline]->d_name, xmax - 2);
      waddch(dsp->subwnd, '~'); 
    }
    else {
      waddstr(dsp->subwnd, dsp->dirs[pline]->d_name);
      wclrtoeol(dsp->subwnd);
    }
  }
  else if (pcur == FILES) {
    if (strlen(dsp->files[pline]->d_name) > xmax) {
      waddnstr(dsp->subwnd, dsp->files[pline]->d_name, xmax - 1);
      waddch(dsp->subwnd, '~'); 
    }
    else {
      waddstr(dsp->subwnd, dsp->files[pline]->d_name);
      wclrtoeol(dsp->subwnd);
    }
  }
  wrefresh(dsp->subwnd);
}

/*
 * Similar to 'Print', but highlights.
 */
void Print_highlight (struct Display *dsp, enum Cursor pcur,
                      int pline, int wndline) {
  wattron(dsp->subwnd, A_REVERSE);
  Print(dsp, pcur, pline, wndline);
  wattroff(dsp->subwnd, A_REVERSE);
  wrefresh(dsp->subwnd);
}

int Show_display(struct Display *dsp) {
  int ymax, xmax;
  int k;
  
  if (dsp->cur == NOCONTENT)
    return -1;
 
  werase(dsp->subwnd); 
  wrefresh(dsp->subwnd);  
  /* Reset the selected item to the beginning and print it*/
  dsp->cur = DIRS;
  dsp->line = 0;
  dsp->hl_line = 0;
  Print_highlight(dsp, DIRS, 0, 0); /* first: dsp->dirs[0]->d_name = ".." */

  getmaxyx(dsp->subwnd, ymax, xmax);
  for (k = 1; k < dsp->size_dirs && k < ymax; k++)
    Print(dsp, DIRS, k, k);
  for (int i = 0; i < dsp->size_files && k < ymax; i++, k++)
    Print(dsp, FILES, i, k);
  return k;
}

void Set_highlight(struct Display *dsp) {
  if (dsp->cur == NOCONTENT)
    return;

  Print_highlight(dsp, dsp->cur, dsp->line, dsp->hl_line);
}

void Reset_highlight(struct Display *dsp) {
  if (dsp->cur == NOCONTENT)
    return;

  Print(dsp, dsp->cur, dsp->line, dsp->hl_line);
}

void Up(struct Display *dsp) {
  if (dsp->cur == NOCONTENT)
    return;
  if (dsp->cur == DIRS && dsp->line == 0)
    return;

  Reset_highlight(dsp);
  if (dsp->line > 0) /* And dsp->cur == FILES */
    dsp->line--;
  else {
    dsp->cur = DIRS;
    dsp->line = dsp->size_dirs - 1;
  }
  if (dsp->hl_line == 0) {
    wscrl(dsp->subwnd, -1);
    wrefresh(dsp->subwnd);
  }
  else
    dsp->hl_line--;
  Set_highlight(dsp);
}

void Down(struct Display *dsp) {
  int ymax, xmax;
  
  if (dsp->cur == NOCONTENT)
    return;
  if (dsp->cur == FILES && dsp->line == dsp->size_files - 1 ||
      dsp->cur == DIRS && dsp->size_files == 0 && dsp->line == dsp->size_dirs-1)
    return;
  
  getmaxyx(dsp->subwnd, ymax, xmax);
  Reset_highlight(dsp);
  if (dsp->cur == DIRS && dsp->line == dsp->size_dirs - 1) {
    dsp->cur = FILES;
    dsp->line = 0;
  }
  else
    dsp->line++;
  if (dsp->hl_line == ymax - 1) {
    wscrl(dsp->subwnd, 1);
    wrefresh(dsp->subwnd);
  }
  else
    dsp->hl_line++;
  Set_highlight(dsp);
}
