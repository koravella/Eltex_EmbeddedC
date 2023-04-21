#include <stdio.h>

void ByteOutput(int num) {
  for(long unsigned int i = 0; i < sizeof(num); i++) {
    char out = (num >> (8 * i)) & 0xFF;
    printf("%ld byte: %hhx\n", i, out);
  }
}

int main(void) {
  int num = 0xAABBCCDD;
  ByteOutput(num);
  printf("changing the low byte to EE\n");
  num = (num & 0xFFFFFF00) | 0x000000EE;
  ByteOutput(num);
}
