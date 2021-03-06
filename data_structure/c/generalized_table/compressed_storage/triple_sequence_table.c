#include <stdio.h>

// 数据结构中，提供针对某些特殊矩阵的压缩存储结构。这里所说的特殊矩阵，主要分为以下两类：
//  含有大量相同数据元素的矩阵，比如对称矩阵；
//  含有大量0元素的矩阵，比如稀疏矩阵、上（下）三角矩阵；
// 针对以上两类矩阵，数据结构的压缩存储思想是：矩阵中的相同数据元素（包括元素0）只存储一个。

// 数据元素沿主对角线对应相等，这类矩阵称为对称矩阵。
// 可以使用一维数组存储对称矩阵。由于矩阵中沿对角线两侧的数据相等，因此数组中只需存储对角线一侧（包含对角线）的数据即可。
// 对称矩阵的实现过程是(最终求得的k值即为该元素存储到数组中的位置，矩阵中元素的行标和列标都从1开始):
//  若存储下三角中的元素，只需将各元素所在的行标i和列标j代入下面的公式：k=i*(i-1)/2 + j - 1
//  存储上三角的元素要将各元素的行标i和列标j代入另一个公式：k=j*(j-1)/2 + i - 1
// 以上两个公式既是用来存储矩阵中元素的，也用来从数组中提取矩阵相应位置的元素:
//  1 2 3
//  2 4 5
//  3 5 6
// a[5] = {1,2,4,3,5,6}; 取下三角的(3,1)处的元素，k=3*(3-1)/2+1-1=3,a[3]=4

// 主对角线下的数据元素全部相同的矩阵为上三角矩阵，主对角线上元素全部相同的矩阵为下三角矩阵。
// 上（下）三角矩阵采用对称矩阵的方式存储上（下）三角的数据（元素0不用存储）。

// 如果矩阵中分布有大量的元素0，即非0元素非常少，这类矩阵称为稀疏矩阵。
// 压缩存储稀疏矩阵的方法是：只存储矩阵中的非0元素，与前面的存储方法不同，稀疏矩阵非0元素的存储需同时存储该元素所在矩阵中的行标和列标。

// 对于以上3种特殊的矩阵，对称矩阵和上下三角矩阵的实现方法是相同的，且实现过程比较容易，仅需套用公式即可。
// 稀疏矩阵的压缩存储，数据结构提供有3种具体实现方式：
//  三元组顺序表；
//  行逻辑链接的顺序表；
//  十字链表；

// 稀疏矩阵的压缩存储，至少需要存储以下信息：
//  矩阵中各非0元素的值，以及所在矩阵中的行标和列标；
//  矩阵的总行数和总列数；

//  1 0 0
//  0 0 5
//  3 0 0
// 上面是一个稀疏矩阵，若对其进行压缩存储，矩阵中各非0元素的存储状态
// (1 1 1) (2 3 5) (3 1 3)

// 三元组结构体
typedef struct {
  int i, j;  // 行标i，列标j
  int data;  // 元素值
} triple;

#define number 3
// 矩阵的结构表示
typedef struct {
  triple data[number];  // 存储该矩阵中所有非0元素的三元组
  // n和m分别记录矩阵的行数和列数，num记录矩阵中所有的非0元素的个数
  int n, m, num;
} TSMatrix;

void display(TSMatrix M) {
  for (int i = 1; i <= M.n; i++) {
    for (int j = 1; j <= M.m; j++) {
      int value = 0;
      for (int k = 0; k < M.num; k++) {
        if (i == M.data[k].i && j == M.data[k].j) {
          printf("%d ", M.data[k].data);
          value = 1;
          break;
        }
      }
      if (value == 0) printf("0 ");
    }
    printf("\n");
  }
}

int main() {
  TSMatrix M;
  M.m = 3;
  M.n = 3;
  M.num = 3;
  M.data[0].i = 1;
  M.data[0].j = 1;
  M.data[0].data = 1;
  M.data[1].i = 2;
  M.data[1].j = 3;
  M.data[1].data = 5;
  M.data[2].i = 3;
  M.data[2].j = 1;
  M.data[2].data = 3;
  display(M);
  return 0;
}
