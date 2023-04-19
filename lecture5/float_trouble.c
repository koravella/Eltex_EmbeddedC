#include <stdio.h>

int main(void) {
    float var = 0.0f, last = -1.0f;
  while (var != last) {
    last = var;
    var += 1.0f;
    printf("%f\n", var);
  }
}

