#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define err_exit(msg)   do {perror(msg); exit(EXIT_FAILURE);} while (0)

int main(void) {
  int rd_fd, wr_fd;
  char msg[] = "\"Hi from program 2\"\n";
  char buf;
  int st;

  if((rd_fd = open(".pipe_1_to_2", O_RDONLY)) == -1)
    err_exit("open");
  printf("Program 2 reads ");
  while ((st = read(rd_fd, &buf, 1)) != 0) {
    if (st == -1)
      err_exit("read");
    putchar(buf);
  }
  if (close(rd_fd) == -1)
    err_exit("close");

  if((wr_fd = open(".pipe_2_to_1", O_WRONLY)) == -1)
    err_exit("open");
  printf("Program 2 writes %s", msg);
  if (write(wr_fd, msg, strlen(msg)) == -1)
    err_exit("write");
  if (close(wr_fd) == -1)
    err_exit("close");

  return EXIT_SUCCESS;
}
