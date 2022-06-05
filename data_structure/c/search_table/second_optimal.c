#include <stdio.h>

// 在静态查找表中对特定关键字进行顺序查找、折半查找或者分块查找，
// 都是在查找表中各关键字被查找概率相同的前提下进行的。
// 在查找表中各关键字查找概率不相同的情况下，对于使用折半查找算法，
// 按照之前的方式进行，其查找的效率并不一定是最优的。
// 在查找表中各关键字查找概率不同时，要考虑建立一棵查找性能最佳的判定树。
// 若在只考虑查找成功的情况下，描述查找过程的判定树其带权路径长度之和（用PH表示）最小时，
// 查找性能最优，称该二叉树为静态最优查找树。
// 带权路径之和的计算公式为：PH = 所有结点所在的层次数 * 每个结点对应的概率值。
// 但是由于构造最优查找树花费的时间代价较高，
// 而且有一种构造方式创建的判定树的查找性能同最优查找树仅差1%-2%，
// 称这种极度接近于最优查找树的二叉树为次优查找树。

typedef int KeyType;  // 定义关键字类型
typedef struct {
  KeyType key;
} ElemType;  // 定义元素类型
typedef struct BiTNode {
  ElemType data;
  struct BiTNode *lchild, *rchild;
} BiTNode, *BiTree;

int i;
int min;
int dw;
// 创建次优查找树，R数组为查找表，sw数组为存储的各关键字的概率（权值），
// low和high表示的sw数组中的权值的范围
void SecondOptimal(BiTree T, ElemType R[], float sw[], int low, int high) {
  // 由有序表R[low...high]及其累计权值表sw（其中sw[0]==0）递归构造次优查找树
  i = low;
  min = abs(sw[high] - sw[low]);
  dw = sw[high] + sw[low - 1];
  // 选择最小的△Pi值
  for (int j = low + 1; j <= high; j++) {
    if (abs(dw - sw[j] - sw[j - 1]) < min) {
      i = j;
      min = abs(dw - sw[j] - sw[j - 1]);
    }
  }

  T = (BiTree)malloc(sizeof(BiTNode));
  T->data = R[i];  // 生成结点（第一次生成根）
  if (i == low)
    T->lchild = NULL;  // 左子树空
  else
    SecondOptimal(T->lchild, R, sw, low, i - 1);  // 构造左子树
  if (i == high)
    T->rchild = NULL;  // 右子树空
  else
    SecondOptimal(T->rchild, R, sw, i + 1, high);  // 构造右子树
}

// 由于使用次优查找树和最优查找树的性能差距很小，构造次优查找树的算法的时间复杂度为O(nlogn)，
// 因此可以使用次优查找树表示概率不等的查找表对应的静态查找表（又称为静态树表）。