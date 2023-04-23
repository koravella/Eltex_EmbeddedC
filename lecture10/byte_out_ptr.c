#include <stdio.h>

void ByteOutput(int num) {
  char *ptr = (char*) &num;
  for(long unsigned int i = 0; i < sizeof(num); i++) {
      printf("%ld byte: %hhx\n", i, ptr[i]);
  }
  return;
}

int main(void) {
  int num = 0xAABBCCDD;
  ByteOutput(num);
  printf("changing the high byte to EE\n");
  char *ptr = (char*) &num;
  ptr[3] = 0xEE;
  ByteOutput(num);
  return 0;
}
