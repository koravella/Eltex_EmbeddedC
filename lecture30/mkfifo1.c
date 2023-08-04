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
  char msg[] = "\"Hello from program 1\"\n";
  char buf;
  int st;

  if (mkfifo(".pipe_1_to_2", S_IRUSR | S_IWUSR) == -1)
    err_exit("mkfifo");
  if (mkfifo(".pipe_2_to_1", S_IRUSR | S_IWUSR) == -1)
    err_exit("mkfifo");

  if((wr_fd = open(".pipe_1_to_2", O_WRONLY)) == -1)
    err_exit("open");
  printf("Program 1 writes %s", msg);
  if (write(wr_fd, msg, strlen(msg)) == -1)
    err_exit("write");
  if (close(wr_fd) == -1)
    err_exit("close");

  if((rd_fd = open(".pipe_2_to_1", O_RDONLY)) == -1)
    err_exit("open");
  printf("Program 1 reads ");
  while ((st = read(rd_fd, &buf, 1)) != 0) {
    if (st == -1)
      err_exit("read");
    putchar(buf);
  }
  if (close(rd_fd) == -1)
    err_exit("close");


  if (remove(".pipe_1_to_2") == -1)
    err_exit("remove");
  if(remove(".pipe_2_to_1") == -1)
    err_exit("remove");
  return EXIT_SUCCESS;
}
