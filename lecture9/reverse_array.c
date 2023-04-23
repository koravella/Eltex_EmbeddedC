#include <stdio.h>

#define N 5

void Swap(int *var1, int *var2) {
  int tmp;
  tmp = *var1;
  *var1 = *var2;
  *var2 = tmp;
  return;
}

int main(void) {
  int arr[N];
  for (size_t i = 0; i < N; i++) {
    arr[i] = i + 1;
  }

  for (size_t i = 0; i < N/2; i++) {
    Swap(&arr[i], &arr[N-1-i]);
  }

 for (size_t i = 0; i < N; i++) {
    printf("%d ", arr[i]);
  }
  printf("\n");
  return 0;
}
