#include <stdio.h>
#include <stdlib.h>

// 平衡二叉树，又称为AVL树。实际上就是遵循以下两个特点的二叉树：
//  每棵子树中的左子树和右子树的深度差不能超过1；
//  二叉树中每棵子树都要求是平衡二叉树；
// 其实就是在二叉树的基础上，若树中每棵子树都满足其左子树和右子树的深度差都不超过1，则这棵二叉树就是平衡二叉树。
// 平衡因子：每个结点都有其各自的平衡因子，表示的就是其左子树深度同右子树深度的差。平衡二叉树中各结点平衡因子的取值只可能是：0、1和-1。

// 为了排除动态查找表中不同的数据排列方式对算法性能的影响，需要考虑在不会破坏二叉排序树本身结构的前提下，将二叉排序树转化为平衡二叉树。
// 当二叉排序树的平衡性被打破时，就如同扁担的两头出现了一头重一头轻的现象，此时只需要改变扁担的支撑点（树的树根），就能使其重新归为平衡。
// 当平衡二叉树由于新增数据元素导致整棵树的平衡遭到破坏时，就需要根据实际情况做出适当的调整，假设距离插入结点最近的“不平衡因子”为a。则调整的规律可归纳为以下4种情况：
// 单向右旋平衡处理：若由于结点a的左子树为根结点的左子树上插入结点，导致结点a的平衡因子由1增至2，致使以a为根结点的子树失去平衡，则只需进行一次向右的顺时针旋转。
// 单向左旋平衡处理：如果由于结点a的右子树为根结点的右子树上插入结点，导致结点a的平衡因子由-1变为-2，则以a为根结点的子树需要进行一次向左的逆时针旋转。
// 双向旋转（先左后右）平衡处理：如果由于结点a的左子树为根结点的右子树上插入结点，导致结点a平衡因子由1增至2，致使以a为根结点的子树失去平衡，则需要进行两次旋转操作。
// 双向旋转（先右后左）平衡处理：如果由于结点a的右子树为根结点的左子树上插入结点，导致结点a平衡因子由-1变为-2，致使以a为根结点的子树失去平衡，则需要进行两次旋转操作。

// 分别定义平衡因子数
#define LH +1
#define EH 0
#define RH -1
typedef int ElemType;
typedef enum { false, true } bool;
// 定义二叉排序树
typedef struct BSTNode {
  ElemType data;
  int bf;  // balance flag
  struct BSTNode *lchild, *rchild;
} * BSTree, BSTNode;
// 对以p为根结点的二叉树做右旋处理，令p指针指向新的树根结点
void R_Rotate(BSTree* p) {
  // 借助文章中的图 5 所示加以理解，其中结点A为p指针指向的根结点
  BSTree lc = (*p)->lchild;
  (*p)->lchild = lc->rchild;
  lc->rchild = *p;
  *p = lc;
}
// 对以p为根结点的二叉树做左旋处理，令p指针指向新的树根结点
void L_Rotate(BSTree* p) {
  BSTree rc = (*p)->rchild;
  (*p)->rchild = rc->lchild;
  rc->lchild = *p;
  *p = rc;
}
// 对以指针T所指向结点为根结点的二叉树作左子树的平衡处理，令指针T指向新的根结点
void LeftBalance(BSTree* T) {
  BSTree lc, rd;
  lc = (*T)->lchild;
  // 查看以T的左子树为根结点的子树，失去平衡的原因，如果bf值为1，则说明添加在左子树为根结点的左子树中，需要对其进行右旋处理；
  // 反之，如果bf值为-1，说明添加在以左子树为根结点的右子树中，需要进行双向先左旋后右旋的处理
  switch (lc->bf) {
    case LH:
      (*T)->bf = lc->bf = EH;
      R_Rotate(T);
      break;
    case RH:
      rd = lc->rchild;
      switch (rd->bf) {
        case LH:
          (*T)->bf = RH;
          lc->bf = EH;
          break;
        case EH:
          (*T)->bf = lc->bf = EH;
          break;
        case RH:
          (*T)->bf = EH;
          lc->bf = LH;
          break;
      }
      rd->bf = EH;
      L_Rotate(&(*T)->lchild);
      R_Rotate(T);
      break;
  }
}
// 右子树的平衡处理同左子树的平衡处理完全类似
void RightBalance(BSTree* T) {
  BSTree lc, rd;
  lc = (*T)->rchild;
  switch (lc->bf) {
    case RH:
      (*T)->bf = lc->bf = EH;
      L_Rotate(T);
      break;
    case LH:
      rd = lc->lchild;
      switch (rd->bf) {
        case LH:
          (*T)->bf = EH;
          lc->bf = RH;
          break;
        case EH:
          (*T)->bf = lc->bf = EH;
          break;
        case RH:
          (*T)->bf = EH;
          lc->bf = LH;
          break;
      }
      rd->bf = EH;
      R_Rotate(&(*T)->rchild);
      L_Rotate(T);
      break;
  }
}
int InsertAVL(BSTree* T, ElemType e, bool* taller) {
  // 如果本身为空树，则直接添加e为根结点
  if ((*T) == NULL) {
    (*T) = (BSTree)malloc(sizeof(BSTNode));
    (*T)->bf = EH;
    (*T)->data = e;
    (*T)->lchild = NULL;
    (*T)->rchild = NULL;
    *taller = true;
  }
  // 如果二叉排序树中已经存在e ，则不做任何处理
  else if (e == (*T)->data) {
    *taller = false;
    return 0;
  }
  // 如果e小于结点T的数据域，则插入到T的左子树中
  else if (e < (*T)->data) {
    // 如果插入过程，不会影响树本身的平衡，则直接结束
    if (!InsertAVL(&(*T)->lchild, e, taller)) return 0;
    // 判断插入过程是否会导致整棵树的深度 +1
    if (*taller) {
      // 判断根结点T的平衡因子是多少，由于是在其左子树添加新结点的过程中导致失去平衡，所以当T结点的平衡因子本身为1时，需要进行左子树的平衡处理，否则更新树中各结点的平衡因子数
      switch ((*T)->bf) {
        case LH:
          LeftBalance(T);
          *taller = false;
          break;
        case EH:
          (*T)->bf = LH;
          *taller = true;
          break;
        case RH:
          (*T)->bf = EH;
          *taller = false;
          break;
      }
    }
  }
  // 同样，当e>T->data时，需要插入到以T为根结点的树的右子树中，同样需要做和以上同样的操作
  else {
    if (!InsertAVL(&(*T)->rchild, e, taller)) return 0;
    if (*taller) {
      switch ((*T)->bf) {
        case LH:
          (*T)->bf = EH;
          *taller = false;
          break;
        case EH:
          (*T)->bf = RH;
          *taller = true;
          break;
        case RH:
          RightBalance(T);
          *taller = false;
          break;
      }
    }
  }
  return 1;
}
// 判断现有平衡二叉树中是否已经具有数据域为e的结点
bool FindNode(BSTree root, ElemType e, BSTree* pos) {
  BSTree pt = root;
  (*pos) = NULL;
  while (pt) {
    if (pt->data == e) {
      // 找到节点，pos指向该节点并返回true
      (*pos) = pt;
      return true;
    } else if (pt->data > e) {
      pt = pt->lchild;
    } else
      pt = pt->rchild;
  }
  return false;
}
// 中序遍历平衡二叉树
void InorderTra(BSTree root) {
  if (root->lchild) InorderTra(root->lchild);

  printf("%d ", root->data);

  if (root->rchild) InorderTra(root->rchild);
}

// 使用平衡二叉树进行查找操作的时间复杂度为O(logn)。

int main() {
  int i, nArr[] = {1, 23, 45, 34, 98, 9, 4, 35, 23};
  BSTree root = NULL, pos;
  bool taller;
  // 用nArr查找表构建平衡二叉树（不断插入数据的过程）
  for (i = 0; i < 9; i++) {
    InsertAVL(&root, nArr[i], &taller);
  }
  // 中序遍历输出
  InorderTra(root);
  // 判断平衡二叉树中是否含有数据域为103的数据
  if (FindNode(root, 103, &pos))
    printf("\n%d\n", pos->data);
  else
    printf("\nNot find this Node\n");

  // 1 4 9 23 34 35 45 98
  // Not find this Node

  return 0;
}