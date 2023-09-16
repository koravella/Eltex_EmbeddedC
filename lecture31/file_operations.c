#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 5

#define err_exit(msg)   do {perror(msg); exit(EXIT_FAILURE);} while (0)

int main(void) {
  char str[] = "Test string\n";
  char buf[BUFSIZE];
  int fd;
  int i, cnt;

  if (creat("test", 00700) == -1)
    err_exit("creat");
  if ((fd = open("test", O_WRONLY)) == -1)
    err_exit("open");
  if (write(fd, str, strlen(str)) == -1)
    err_exit("write");
  if (close(fd) == -1)
    err_exit("close");

  if ((fd = open("test", O_RDONLY)) == -1)
    err_exit("open");
  while ((cnt = read(fd, buf, BUFSIZE)) > 0) {
    for (i = 0; i < cnt; i++)
      putchar(buf[i]);
  }
  if (cnt == -1)
    err_exit("read");
  if (close(fd) == -1)
    err_exit("close");
  if (unlink("test") == -1)
    err_exit("unlink");
  return EXIT_SUCCESS;
}
