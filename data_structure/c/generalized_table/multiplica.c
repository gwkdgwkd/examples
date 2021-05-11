#include <stdio.h>

// 矩阵相乘的前提条件是：乘号前的矩阵的列数要和乘号后的矩阵的行数相等。且矩阵的乘法运算没有交换律，即A*B和B*A是不一样的。
// 计算方法是：用矩阵A的第i行和矩阵B中的每一列j对应的数值做乘法运算，乘积一一相加，所得结果即为矩阵C中第i行第j列的值。
// 在稀疏矩阵做乘法运算时，由于本身矩阵中含有的非0元素少，普通算法会出现很多0*0或者k*0或者0*k（k代表非0元素值）的情况。
// 行逻辑链接的顺序表解决矩阵乘积算法:
//  1 首先，找到矩阵A中第一行的非0元素，分别是A11=3和A14=5；（由于行逻辑链接的顺序表中存储的都是非0元素，查找的过程就需要使用记录每行第一个非0元素的首地址的数组来完成）
//  2 用3去和B中对应的第一行中的非0元素相乘，矩阵B中第一行非0元素是B12=2，所以3*2=6 ，因为6是A11和B12相乘的结果，所以暂时存放在C12中；
//    用5去和B中对应的第4行的非 0 元素相乘，由于矩阵 B 中第 4 行没有非 0 元素，所以，第一行的计算结束；
//  3 以此类推。
// 现在，解决问题的关键在于，如何知道顺序表中存放的非0元素是哪一行的呢？
//  解决方案：由于使用的是行逻辑链接的顺序表，所以，已经知道了每一个矩阵中的每一行有多少个非0元素，而且第一行的第一个非0元素的位置一定是1。
//  所以，第n行的非0元素的位置范围是：大于或等于第n行第一个元素的位置， 小于第n+1行第一个元素的位置（如果是矩阵的最后一行，小于矩阵中非0元素的个数+1）。
// 当稀疏矩阵Amn和稀疏矩阵Bn 采用行逻辑链接的顺序表做乘法运算时，在矩阵A的列数（矩阵B的行数）n不是很大的情况下，算法的时间复杂度相当于O(m*p)，比普通算法要快很多。

#define MAXSIZE 1200
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
RLSMatrix MultSMatrix(RLSMatrix A, RLSMatrix B, RLSMatrix C) {
  // 如果矩阵A的列数与矩阵B的行数不等，则不能做矩阵乘运算
  if (A.nu != B.mu) return C;
  C.mu = A.mu;
  C.nu = B.nu;
  C.tu = 0;
  // 如果其中任意矩阵的元素个数为零，做乘法元素没有意义，全是0
  if (A.tu * B.tu == 0)
    return C;
  else {
    int arow;
    int ccol;
    // 遍历矩阵A的每一行
    for (arow = 1; arow <= A.mu; arow++) {
      // 创建一个临时存储乘积结果的数组，且初始化为0，遍历每次都需要清空
      int ctemp[MAXRC + 1];
      for (int i = 0; i < MAXRC + 1; i++) {
        ctemp[i] = 0;
      }
      C.rpos[arow] = C.tu + 1;
      // 根据行数，在三元组表中找到该行所有的非0元素的位置
      int tp;
      if (arow < A.mu)
        tp = A.rpos[arow +
                    1];  // 获取矩阵A的下一行第一个非零元素在data数组中位置
      else
        tp = A.tu + 1;  // 若当前行是最后一行，则取最后一个元素+1
      int p;
      int brow;
      // 遍历当前行的所有的非0元素
      for (p = A.rpos[arow]; p < tp; p++) {
        brow =
            A.data[p].j;  // 取该非0元素的列数，便于去B中找对应的做乘积的非0元素
        int t;
        // 判断如果对于A中非0元素，找到矩阵B中做乘法的那一行中的所有的非0元素
        if (brow < B.mu)
          t = B.rpos[brow + 1];
        else
          t = B.tu + 1;
        int q;
        // 遍历找到的对应的非0元素，开始做乘积运算
        for (q = B.rpos[brow]; q < t; q++) {
          // 得到的乘积结果，每次和ctemp数组中相应位置的数值做加和运算
          ccol = B.data[q].j;
          ctemp[ccol] += A.data[p].e * B.data[q].e;
        }
      }
      // 矩阵C的行数等于矩阵A的行数，列数等于矩阵B的列数，所以，得到的ctemp存储的结果，也会在C的列数的范围内
      for (ccol = 1; ccol <= C.nu; ccol++) {
        // 由于结果可以是0，而0不需要存储，所以在这里需要判断
        if (ctemp[ccol]) {
          // 不为0，则记录矩阵中非0元素的个数的变量tu要+1；且该值不能超过存放三元素数组的空间大小
          if (++C.tu > MAXSIZE)
            return C;
          else {
            C.data[C.tu].e = ctemp[ccol];
            C.data[C.tu].i = arow;
            C.data[C.tu].j = ccol;
          }
        }
      }
    }
    return C;
  }
}
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
  RLSMatrix M, N, T;
  M.tu = 4;
  M.mu = 3;
  M.nu = 4;
  M.rpos[1] = 1;
  M.rpos[2] = 3;
  M.rpos[3] = 4;
  M.data[1].e = 3;
  M.data[1].i = 1;
  M.data[1].j = 1;
  M.data[2].e = 5;
  M.data[2].i = 1;
  M.data[2].j = 4;
  M.data[3].e = -1;
  M.data[3].i = 2;
  M.data[3].j = 2;
  M.data[4].e = 2;
  M.data[4].i = 3;
  M.data[4].j = 1;
  N.tu = 4;
  N.mu = 4;
  N.nu = 2;
  N.rpos[1] = 1;
  N.rpos[2] = 2;
  N.rpos[3] = 3;
  N.rpos[4] = 5;
  N.data[1].e = 2;
  N.data[1].i = 1;
  N.data[1].j = 2;
  N.data[2].e = 1;
  N.data[2].i = 2;
  N.data[2].j = 1;
  N.data[3].e = -2;
  N.data[3].i = 3;
  N.data[3].j = 1;
  N.data[4].e = 4;
  N.data[4].i = 3;
  N.data[4].j = 2;

  T = MultSMatrix(M, N, T);
  for (int i = 1; i <= T.tu; i++) {
    printf("(%d,%d,%d)\n", T.data[i].i, T.data[i].j, T.data[i].e);
  }
  // (1,2,6)
  // (2,1,-1)
  // (3,2,4)

  display(M);
  // 3 0 0 5
  // 0 -1 0 0
  // 2 0 0 0
  display(N);
  // 0 2
  // 1 0
  // -2 4
  // 0 0
  display(T);
  // 0 6
  // -1 0
  // 0 4

  return 0;
}
