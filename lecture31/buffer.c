#include "buffer.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define INIT_LINE_SIZE 128  /* Initial size of array 'symb' in 'struct Line' */
#define INIT_NUM_LINES 128  /* Initial size of array 'line' in 'struct Buffer'*/

struct Line* New_line() {
  struct Line *line;

  line = (struct Line*) malloc(sizeof(struct Line));
  line->size = 0;
  line->max_size = INIT_LINE_SIZE;
  line->symb = (char*) malloc(sizeof(char) * line->max_size);
  return line; 
}

struct Buffer* New_buffer() {
  struct Buffer *buf;

  buf = (struct Buffer*) malloc(sizeof(struct Buffer));
  buf->nlines = 0;
  buf->max_nlines = INIT_NUM_LINES;
  buf->line = (struct Line**) malloc(sizeof(struct Line*) * buf->max_nlines); 
  for (int i = 0; i < buf->max_nlines; i++)
    buf->line[i] = New_line();
  return buf;
}

/*
 * Increases the size of the array 'line->symb' by 2 times.
 */
void Expand_line(struct Line *line) {
  line->max_size *= 2;
  line->symb = (char*) realloc(line->symb, sizeof(char) * line->max_size);
}

/*
 * Increases the size of the array 'buf->line' by 2 times.
 */
void Expand_buffer(struct Buffer *buf) {
  int old_max_nlines;

  old_max_nlines = buf->max_nlines;
  buf->max_nlines *= 2;
  buf->line = (struct Line**) realloc(buf->line,
                                      sizeof(struct Line*) * buf->max_nlines);
  for (int i = old_max_nlines; i < buf->max_nlines; i++)
    buf->line[i] = New_line();
}

/*
 * Reads text character by character.
 */
void Read_file(struct Buffer *buf, int fd) {
  char ch;

  while (read(fd, &ch, 1) != 0) {
    struct Line *cur = buf->line[buf->nlines];
    cur->symb[cur->size] = ch;
    cur->size++;
    if (cur->size == cur->max_size)
      Expand_line(cur);
    if (ch == '\n') {
      buf->nlines++;
      if (buf->nlines == buf->max_nlines)
        Expand_buffer(buf);
    }
  }
}

/*
 * Writes text character by character.
 */
void Write_file(struct Buffer *buf, int fd) {
  for (int i = 0; i < buf->nlines; i++)
    write(fd, buf->line[i]->symb, buf->line[i]->size);
}

void Delete_line(struct Line *line) {
  free(line->symb);
  free(line);
}

void Delete_buffer(struct Buffer *buf) {
  for (int i = 0; i < buf->max_nlines; i++)
    Delete_line(buf->line[i]);
  free(buf->line);
  free(buf);
}
