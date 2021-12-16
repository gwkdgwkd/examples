#include <stdio.h>

// 要想使N个皇后不相互攻击，应将它们放置在不同的行、不同的列、还不能位于同一条45°（或135°）角的斜线上。
// 回溯算法解决N皇后问题的具体思路是：
// 将N个皇后逐一放置在不同的行，以“回溯”的方式逐一测试出每行皇后所在行的具体位置，最终确定所有皇后的位置。

#define N 20    // 皇后的数量
int q[N];       // 各行皇后所在的列
int count = 0;  // 统计N皇后问题解的个数
// 输出N皇后问题的解决方案
void print(int n) {
  int i, j;
  count++;
  printf("第%d个解：\n", count);
  for (i = 1; i <= n; i++)  // 行
  {
    for (j = 1; j <= n; j++)  // 列
    {
      if (q[i] != j)
        printf("x");
      else
        printf("Q");
    }
    printf("\n");
  }
  printf("\n");
}
// 检验第k行第j列上是否可以摆放皇后
int isSafe(int k, int j) {
  int i;
  for (i = 1; i < k; i++) {
    // 如果有其它皇后位置同一列上，或者位于该位置的斜线位置上，则该位置无法使用
    if (q[i] == j || abs(i - k) == abs(q[i] - j)) return 0;
  }
  return 1;
}
// 放置皇后到棋盘上
void n_queens(int k, int n) {
  int j;
  if (k > n)  // 递归的出口
    print(n);
  else {
    for (j = 1; j <= n; j++)  // 试探第k行的每一列，找到符合要求的列
    {
      if (isSafe(k, j)) {
        q[k] = j;
        // 在确认第k行皇后位置的前提下，继续测试下一行皇后的位置
        n_queens(k + 1, n);
      }
    }
  }
}
int main() {
  int n;
  printf("请输入皇后个数：");
  scanf("%d", &n);
  n_queens(1, n);
  printf("共有%d种不同的排列\n", count);

  return 0;
}