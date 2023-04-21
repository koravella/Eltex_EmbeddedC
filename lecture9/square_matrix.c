#include <stdio.h>

#define N 5

int main(void) {
  int matrix[N][N];
  for (size_t i = 0; i < N; i++) {
    for (size_t j = 0; j < N; j++) {
      matrix[i][j] = i*N + j + 1;
    }
  }

 for (size_t i = 0; i < N; i++) {
    for (size_t j = 0; j < N; j++) {
      printf("%d\t", matrix[i][j]);
    }
    printf("\n");
  }
  return 0;
}
