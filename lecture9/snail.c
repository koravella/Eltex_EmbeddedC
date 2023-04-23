#include <stdio.h>

#define N 5

int main(void) {
  int matrix[N][N];
  int dir_row = 0, dir_col = 1;
  int col = -1, row = 0;
  int num = 0;
  for (int i = 2 * N; i > 1; i--) {
    for (int j = 0; j < i / 2; j++) {
      col += dir_col;
      row += dir_row;
      num++;
      matrix[row][col] = num;
    }
    int tmp = dir_col;
    dir_col = -1 * dir_row;
    dir_row = tmp;
  }

  for (size_t i = 0; i < N; i++) {
    for (size_t j = 0; j < N; j++) {
      printf("%d\t", matrix[i][j]);
    }
    printf("\n");
  }
  return 0;
}
