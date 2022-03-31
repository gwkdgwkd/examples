#include <stdio.h>
#include <stdlib.h>

// 对于压缩存储稀疏矩阵，无论是使用三元组顺序表，还是使用行逻辑链接的顺序表，归根结底是使用数组存储稀疏矩阵。
// 介于数组"不利于插入和删除数据"的特点，以上两种压缩存储方式都不适合解决类似"向矩阵中添加或删除非0元素"的问题。
// 使用十字链表压缩存储稀疏矩阵时，矩阵中的各行各列都各用一各链表存储，与此同时，
// 所有行链表的表头存储到一个数组（rhead），所有列链表的表头存储到另一个数组（chead）中。
// 该存储方式采用的是"链表+数组"结构。

// 链表中节点的结构:
//  |行标|列标|结点值|
//  |指针域A|指针域B |
// 指针域A存储的是矩阵中结点所在列的下一个结点的地址（称为 “down域”）；
// 指针域B存储的是矩阵中该结点所在行的下一个结点的地址（称为 “right域”）；

typedef struct OLNode {
  int i, j, e;  // 矩阵三元组i代表行 j代表列 e代表当前位置的数据
  struct OLNode *right, *down;  // 指针域 右指针 下指针
} OLNode, *OLink;
typedef struct {
  OLink *rhead, *chead;  // 行和列链表头指针
  int mu, nu, tu;        // 矩阵的行数,列数和非零元的个数
} CrossList;

CrossList CreateMatrix_OL(CrossList M) {
  int m, n, t;
  int i, j, e;
  OLNode *p, *q;
  printf("输入矩阵的行数、列数和非0元素个数：");
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
    // 链接到行的指定位置
    if (NULL == M.rhead[i] || M.rhead[i]->j > j) {
      p->right = M.rhead[i];
      M.rhead[i] = p;
    } else {
      for (q = M.rhead[i]; (q->right) && q->right->j < j; q = q->right)
        ;
      p->right = q->right;
      q->right = p;
    }
    // 链接到列的指定位置
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

void display(CrossList M) {
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

int main() {
  CrossList M;
  M.rhead = NULL;
  M.chead = NULL;
  M = CreateMatrix_OL(M);
  printf("输出矩阵M:\n");
  display(M);
  // 2       2       3
  // 3       2       5
  // 2       3       4

  return 0;
}