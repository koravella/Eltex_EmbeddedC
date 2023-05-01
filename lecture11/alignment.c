#include <stdio.h>

int main(void) {
  struct TestAlignment {
    char a;
    int b;
  };
  struct TestPacked {
    char a;
    int b;
  } __attribute__((packed));
  char str[10] = {'A', 0, 0, 0, 0, 'B', 0, 0, 0, 0};

  struct TestAlignment *ptr1 = (struct TestAlignment*) str;
  printf("Alignment:\n");
  // Potentially segmentation fault 
  for (size_t i = 0; i <2; i++) {
    printf("%c %d:\t0x%.2X 0x%.8X\n", 
           ptr1[i].a, ptr1[i].b, ptr1[i].a, ptr1[i].b);
  }

  struct TestPacked *ptr2 = (struct TestPacked*) str;
  printf("Packed:\n");
  for (size_t i = 0; i <2; i++) {
    printf("%c %d:\t0x%.2X 0x%.8X\n", 
           ptr2[i].a, ptr2[i].b, ptr2[i].a, ptr2[i].b);
  }
  return 0;
}
