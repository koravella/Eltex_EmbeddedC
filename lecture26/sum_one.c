#include <stdio.h>

#define NUMBER 100000000

long res = 0;

int main(void) {
  for (int i = 0; i < NUMBER; i++)
    ++res;

  printf("%ld\n", res);
  return 0;
}
