#include <stdio.h>
#include <stdlib.h>

// 矩阵之间能够进行加法运算的前提条件是：各矩阵的行数和列数必须相等。在行数和列数都相等的情况下，矩阵相加的结果就是矩阵中对应位置的值相加所组成的矩阵。
// 之前所介绍的都是采用顺序存储结构存储三元组，在类似于矩阵的加法运算中，矩阵中的数据元素变化较大（这里的变化主要为：非0元素变为0，0变为非0元素），
// 就需要考虑采用另一种结构——链式存储结构来存储三元组。采用链式存储结构存储稀疏矩阵三元组的方法，称为“十字链表法”。
// 采用十字链表表示矩阵时，矩阵的每一行和每一个列都可以看作是一个单独的链表，而之所以能够表示矩阵，是因为行链表和列链表都分别存储在各自的数组中。
// 存储行链表的数组称为rhead数组；存储列链表的数组称为chead数组。
// 使用十字链表法解决稀疏矩阵的压缩存储的同时，在解决矩阵相加的问题中，对于某个单独的结点来说，算法的时间复杂度为一个常数（全部为选择结构），
// 算法的整体的时间复杂度取决于两矩阵中非0元素的个数。

typedef struct OLNode {
  int i, j, e;  // 矩阵三元组i代表行 j代表列 e代表当前位置的数据
  struct OLNode *right, *down;  // 指针域 右指针 下指针
} OLNode, *OLink;

typedef struct {
  OLink *rhead, *chead;  // 行和列链表头指针
  int mu, nu, tu;        // 矩阵的行数,列数和非零元的个数
} CrossList;

void display(CrossList M) {
  printf("输出测试矩阵:\n");
  printf("M:\n---------------------\ni\tj\te\n---------------------\n");
  for (int i = 1; i <= M.nu; i++) {
    if (NULL != M.chead[i]) {
      OLink p = M.chead[i];
      while (NULL != p) {
        printf("%d\t%d\t%d\n", p->i, p->j, p->e);
        p = p->down;
      }
    }
  }
}

CrossList CreateMatrix_OL(CrossList M) {
  int m, n, t;
  int i, j, e;
  OLNode *p, *q;
  scanf("%d%d%d", &m, &n, &t);
  M.mu = m;
  M.nu = n;
  M.tu = t;
  if (!(M.rhead = (OLink *)malloc((m + 1) * sizeof(OLink))) ||
      !(M.chead = (OLink *)malloc((n + 1) * sizeof(OLink)))) {
    printf("初始化矩阵失败");
    exit(0);
  }
  for (i = 1; i <= m; i++) {
    M.rhead[i] = NULL;
  }
  for (j = 1; j <= n; j++) {
    M.chead[j] = NULL;
  }
  for (scanf("%d%d%d", &i, &j, &e); 0 != i; scanf("%d%d%d", &i, &j, &e)) {
    if (!(p = (OLNode *)malloc(sizeof(OLNode)))) {
      printf("初始化三元组失败");
      exit(0);
    }
    p->i = i;
    p->j = j;
    p->e = e;
    if (NULL == M.rhead[i] || M.rhead[i]->j > j) {
      p->right = M.rhead[i];
      M.rhead[i] = p;
    } else {
      for (q = M.rhead[i]; (q->right) && q->right->j < j; q = q->right)
        ;
      p->right = q->right;
      q->right = p;
    }

    if (NULL == M.chead[j] || M.chead[j]->i > i) {
      p->down = M.chead[j];
      M.chead[j] = p;
    } else {
      for (q = M.chead[j]; (q->down) && q->down->i < i; q = q->down)
        ;
      p->down = q->down;
      q->down = p;
    }
  }
  return M;
}

CrossList AddSMatrix(CrossList M, CrossList N) {
  OLNode *pa, *pb;
  OLink *hl = (OLink *)malloc(M.nu * sizeof(OLink));
  OLNode *pre = NULL;
  for (int j = 1; j <= M.nu; j++) {
    hl[j] = M.chead[j];
  }
  for (int i = 1; i <= M.mu; i++) {
    pa = M.rhead[i];
    pb = N.rhead[i];
    while (pb != NULL) {
      OLNode *p = (OLNode *)malloc(sizeof(OLNode));
      p->i = pb->i;
      p->j = pb->j;
      p->e = pb->e;
      p->down = NULL;
      p->right = NULL;
      if (pa == NULL || pa->j > pb->j) {
        if (pre == NULL) {
          M.rhead[p->i] = p;
        } else {
          pre->right = p;
        }
        p->right = pa;
        pre = p;
        if (!M.chead[p->j] || M.chead[p->j]->i > p->i) {
          p->down = M.chead[p->j];
          M.chead[p->j] = p;
        } else {
          p->down = hl[p->j]->down;
          hl[p->j]->down = p;
        }
        hl[p->j] = p;
      } else {
        if (pa->j < pb->j) {
          pre = pa;
          pa = pa->right;
          continue;
        }
        if (pa->j == pb->j) {
          pa->e += pb->e;
          if (pa->e == 0) {
            if (pre == NULL) {
              M.rhead[pa->i] = pa->right;
            } else {
              pre->right = pa->right;
            }
            p = pa;
            pa = pa->right;
            if (M.chead[p->j] == p) {
              M.chead[p->j] = hl[p->j] = p->down;
            } else {
              hl[p->j]->down = p->down;
            }
            free(p);
          }
        }
      }
      pb = pb->right;
    }
  }
  display(M);
  return M;
}

void main() {
  CrossList M, N;
  M.rhead = NULL;
  M.chead = NULL;
  N.rhead = NULL;
  N.chead = NULL;
  printf("输入测试矩阵M:\n");
  M = CreateMatrix_OL(M);
  printf("输入测试矩阵N:\n");
  N = CreateMatrix_OL(N);
  M = AddSMatrix(M, N);
  printf("矩阵相加的结果为:\n");

  display(M);
}

// 输入测试矩阵M:
// 3 3 3
// 1 2 1
// 2 1 1
// 3 3 1
// 0 0 0
// 输入测试矩阵N:
// 3 3 4
// 1 2 -1
// 1 3 1
// 2 3 1
// 3 1 1
// 0 0 0
// 输出测试矩阵:
// M:
// ---------------------
// i       j       e
// ---------------------
// 2       1       1
// 3       1       1
// 1       3       1
// 2       3       1
// 3       3       1
// 矩阵相加的结果为:
// 输出测试矩阵:
// M:
// ---------------------
// i       j       e
// ---------------------
// 2       1       1
// 3       1       1
// 1       3       1
// 2       3       1
// 3       3       1
