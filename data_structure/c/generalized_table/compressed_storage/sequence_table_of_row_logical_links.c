#include <stdio.h>

// 三元组顺序表每次提取指定元素都需要遍历整个数组，运行效率很低。
// 行逻辑链接的顺序表可以看作是三元组顺序表的升级版，即在三元组顺序表的基础上改善了提取数据的效率。
// 行逻辑链接的顺序表和三元组顺序表的实现过程类似，它们存储矩阵的过程完全相同，
// 都是将矩阵中非0元素的三元组（行标、列标和元素值）存储在一维数组中。
// 但为了提高提取数据的效率，前者在存储矩阵时比后者多使用了一个数组，
// 专门记录矩阵中每行第一个非0元素在一维数组中的位置。

#define MAXSIZE 12500
#define MAXRC 100
#define ElemType int
typedef struct {
  int i, j;    // 行，列
  ElemType e;  // 元素值
} Triple;

typedef struct {
  Triple data[MAXSIZE + 1];
  int rpos[MAXRC + 1];  // 每行第一个非零元素在data数组中的位置
  int mu, nu, tu;       // 行数，列数，元素个数
} RLSMatrix;

void display(RLSMatrix M) {
  for (int i = 1; i <= M.mu; i++) {
    for (int j = 1; j <= M.nu; j++) {
      int value = 0;
      if (i + 1 <= M.mu) {
        for (int k = M.rpos[i]; k < M.rpos[i + 1]; k++) {
          if (i == M.data[k].i && j == M.data[k].j) {
            printf("%d ", M.data[k].e);
            value = 1;
            break;
          }
        }
        if (value == 0) {
          printf("0 ");
        }
      } else {
        for (int k = M.rpos[i]; k <= M.tu; k++) {
          if (i == M.data[k].i && j == M.data[k].j) {
            printf("%d ", M.data[k].e);
            value = 1;
            break;
          }
        }
        if (value == 0) {
          printf("0 ");
        }
      }
    }
    printf("\n");
  }
}
int main(int argc, char* argv[]) {
  RLSMatrix M;
  M.tu = 4;
  M.mu = 3;
  M.nu = 4;
  M.rpos[1] = 1;
  M.rpos[2] = 3;
  M.rpos[3] = 4;
  M.data[1].e = 3;
  M.data[1].i = 1;
  M.data[1].j = 2;
  M.data[2].e = 5;
  M.data[2].i = 1;
  M.data[2].j = 4;
  M.data[3].e = 1;
  M.data[3].i = 2;
  M.data[3].j = 3;
  M.data[4].e = 2;
  M.data[4].i = 3;
  M.data[4].j = 1;

  display(M);
  // 0 3 0 5
  // 0 0 1 0
  // 2 0 0 0

  return 0;
}