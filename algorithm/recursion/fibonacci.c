#include <stdio.h>

// 递归实现斐波那契数列的执行效率是很低的，这与递归的底层实现机制有关

// index表示求数列中第index个位置上的数的值
int fibonacci(int index) {
  // 设置结束递归的限制条件
  if (index == 1 || index == 2) {
    return 1;
  }
  // F(index) = F(index-1) + F(index-2)
  return fibonacci(index - 1) + fibonacci(index - 2);
}

int main() {
  int i;
  // 输出前10个数
  for (i = 1; i <= 10; i++) {
    printf("%d ", fibonacci(i));
  }
  printf("\n");
  return 0;
}