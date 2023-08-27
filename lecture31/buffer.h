/*
 * Defines structures for storing the viewed and edited text.
 */

#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>

/*
 * Auxiliary 'struct Line' for storing a string in an array 'symb' of length
 * 'max_size'. The string is not null-terminated, the real length is determined
 * by 'size'.
 */
struct Line {
  int size;
  int max_size;
  char* symb;
};

/*
 * The 'struct Buffer' stores text in an array 'line' of length 'max_nline'.
 * The actual number of lines used is determined by 'nlines'.
 */
struct Buffer {
  int nlines;
  int max_nlines;
  struct Line** line;
};

struct Line* New_line();
struct Buffer* New_buffer();

/*
 * Increases the allowed number of characters in 'line' to avoid overflow.
 */
void Expand_line(struct Line *line);

/*
 * Increases the allowed number of lines in 'buf' to avoid overflow.
 */
void Expand_buffer(struct Buffer *buf);

/*
 * Reads contents from the file with file descriptor 'fd'.
 */
void Read_file(struct Buffer *buf, int fd);

/*
 * Writes 'buf' contents to the file with file descriptor 'fd'.
 */
void Write_file(struct Buffer *buf, int fd);

void Delete_line(struct Line *line);
void Delete_buffer(struct Buffer *buf);

#endif
