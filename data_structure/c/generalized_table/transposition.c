#include <stdio.h>

#define number 10
typedef struct {
  int i, j;
  int data;
} triple;
typedef struct {
  triple data[10];
  int n, m, num;
} TSMatrix;

// 矩阵（包括稀疏矩阵）的转置，即互换矩阵中所有元素的行标和列标。
// 如果想通过程序实现矩阵的转置，互换行标和列标只是第一步。
// 因为实现矩阵转置的前提是将矩阵存储起来，数据结构中提供了3种存储矩阵的结构，
// 分别是三元组顺序表、行逻辑链接的顺序表和十字链表。
// 如果采用前两种结构，矩阵的转置过程会涉及三元组表也跟着改变的问题。
// 不仅如此，如果矩阵的行数和列数不等，也需要将它们互换。
// 矩阵转置的实现过程需完成以下3步：
// 1.将矩阵的行数和列数互换；
// 2.将三元组表（存储矩阵）中的i列和j列互换，实现矩阵的转置；
// 3.以j列为序，重新排列三元组表中存储各三元组的先后顺序；
// 由于此算法中嵌套使用了两个for循环，时间复杂度为O(n^2)。
TSMatrix transposeMatrix(TSMatrix M, TSMatrix T) {
  T.m = M.n;
  T.n = M.m;
  T.num = M.num;
  if (T.num) {
    int q = 0;
    for (int col = 1; col <= M.m; col++) {
      for (int p = 0; p < M.num; p++) {
        if (M.data[p].j == col) {
          T.data[q].i = M.data[p].j;
          T.data[q].j = M.data[p].i;
          T.data[q].data = M.data[p].data;
          q++;
        }
      }
    }
  }
  return T;
}

// 稀疏矩阵快速转置算法和普通算法的区别仅在于第3步，
// 快速转置能够做到遍历一次三元组表即可完成第3步的工作。
// 稀疏矩阵的快速转置是这样的，在普通算法的基础上增设两个数组（假设分别为array和copt）：
// 1.array数组负责记录原矩阵每一列非0元素的个数。
// 2.copt数组用于计算稀疏矩阵中每列第一个非0元素在新三元组表中存放的位置。
// 3.通常默认第一列首个非0元素存放到新三元组表中的位置为1，
//   cpot[col] = cpot[col-1] + array[col-1]
//   然后通过上面的公式可计算出后续各列首个非0元素存放到新三元组表的位置。
// cpot[col] = cpot[col-1] + array[col-1]的意思是，
// 后一列首个非0元素存放的位置等于前一列首个非0元素的存放位置加上该列非0元素的个数。
// 由此可以看出，copt数组才是最终想要的，而array数组的设立只是为了帮助我们得到copt数组。
// 0 1
// 0 3
// 6 0
// array : 0( ) 1(1) 2(2)
// copt  : 0( ) 1(1) 2(2)
// 稀疏矩阵快速转置算法的时间复杂度为O(n)。
// 即使在最坏的情况下（矩阵中全部都是非0元素），该算法的时间复杂度也才为O(n^2)。
// 实现快速转置算法的函数
TSMatrix fastTransposeMatrix(TSMatrix M, TSMatrix T) {
  // 第1步：行和列置换
  T.m = M.n;
  T.n = M.m;
  T.num = M.num;
  if (T.num) {
    // 计算array数组
    int array[number];
    for (int col = 1; col <= M.m; col++) {
      array[col] = 0;
    }
    for (int t = 0; t < M.num; t++) {
      int j = M.data[t].j;
      array[j]++;
    }
    // 创建并初始化cpot数组
    int cpot[T.m + 1];
    cpot[1] = 1;  // 第一列中第一个非0元素的位置默认为1
    for (int col = 2; col <= M.m; col++) {
      cpot[col] = cpot[col - 1] + array[col - 1];
    }
    // 遍历一次即可实现三元组表的转置
    for (int p = 0; p < M.num; p++) {
      // 提取当前三元组的列数
      int col = M.data[p].j;
      // 根据列数和cpot数组，找到当前元素需要存放的位置
      int q = cpot[col];
      // 转置矩阵的三元组默认从数组下标0开始，而得到的q值是单纯的位置，所以要减1
      T.data[q - 1].i = M.data[p].j;
      T.data[q - 1].j = M.data[p].i;
      T.data[q - 1].data = M.data[p].data;
      // 存放完成后，cpot数组对应的位置要+1，以便下次该列存储下一个三元组
      cpot[col]++;
    }
  }
  return T;
}

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
  M.m = 2;
  M.n = 3;
  M.num = 4;
  M.data[0].i = 1;
  M.data[0].j = 2;
  M.data[0].data = 1;
  M.data[1].i = 2;
  M.data[1].j = 2;
  M.data[1].data = 3;
  M.data[2].i = 3;
  M.data[2].j = 1;
  M.data[2].data = 6;
  M.data[3].i = 3;
  M.data[3].j = 2;
  M.data[3].data = 5;
  TSMatrix T;
  for (int k = 0; k < number; k++) {
    T.data[k].i = 0;
    T.data[k].j = 0;
    T.data[k].data = 0;
  }
  for (int i = 0; i < M.num; i++) {
    printf("(%d,%d,%d)\n", M.data[i].i, M.data[i].j, M.data[i].data);
  }
  // (1,2,1)
  // (2,2,3)
  // (3,1,6)
  // (3,2,5)
  display(M);
  // 0 1
  // 0 3
  // 6 5

  T = transposeMatrix(M, T);
  for (int i = 0; i < T.num; i++) {
    printf("(%d,%d,%d)\n", T.data[i].i, T.data[i].j, T.data[i].data);
  }
  // (1,3,6)
  // (2,1,1)
  // (2,2,3)
  // (2,3,5)
  display(T);
  // 0 0 6
  // 1 3 5

  TSMatrix Q;
  Q = fastTransposeMatrix(M, Q);
  printf("转置矩阵三元组表为：\n");
  for (int i = 0; i < Q.num; i++) {
    printf("(%d,%d,%d)\n", Q.data[i].i, Q.data[i].j, Q.data[i].data);
  }
  // (1,3,6)
  // (2,1,1)
  // (2,2,3)
  // (2,3,5)
  display(Q);
  // 0 0 6
  // 1 3 5

  return 0;
}