#include <stdio.h>
#include <stdlib.h>

// 动态查找表中做查找操作时，若查找成功可以对其进行删除；如果查找失败，即表中无该关键字，可以将该关键字插入到表中。
// 动态查找表的表示方式有多种，二叉排序树（又称为“二叉查找树”），一种使用树结构表示动态查找表的实现方法。
// 二叉排序树要么是空二叉树，要么具有如下特点：
//  二叉排序树中，如果其根结点有左子树，那么左子树上所有结点的值都小于根结点的值；
//  二叉排序树中，如果其根结点有右子树，那么右子树上所有结点的值都大小根结点的值；
//  二叉排序树的左右子树也要求都是二叉排序树；

// 二叉排序树中查找某关键字时，查找过程类似于次优二叉树，在二叉排序树不为空树的前提下，首先将被查找值同树的根结点进行比较，会有3种不同的结果：
//  如果相等，查找成功；
//  如果比较结果为根结点的关键字值较大，则说明该关键字可能存在其左子树中；
//  如果比较结果为根结点的关键字值较小，则说明该关键字可能存在其右子树中；

// 二叉排序树本身是动态查找表的一种表示形式，有时会在查找过程中插入或者删除表中元素，当因为查找失败而需要插入数据元素时，
// 该数据元素的插入位置一定位于二叉排序树的叶子结点，并且一定是查找失败时访问的最后一个结点的左孩子或者右孩子。
// 通过使用二叉排序树对动态查找表做查找和插入的操作，同时在中序遍历二叉排序树时，可以得到有关所有关键字的一个有序的序列。
// 一个无序序列可以通过构建一棵二叉排序树，从而变成一个有序序列。

#define TRUE 1
#define FALSE 0
#define ElemType int
#define KeyType int
// 二叉排序树的节点结构定
typedef struct BiTNode {
  int data;
  struct BiTNode *lchild, *rchild;
} BiTNode, *BiTree;

// 二叉排序树查找算法
int SearchBST(BiTree T, KeyType key, BiTree f, BiTree *p) {
  //如果T指针为空，说明查找失败，令p指针指向查找过程中最后一个叶子结点，并返回查找失败的信息
  if (!T) {
    *p = f;
    return FALSE;
  } else if (key == T->data) {
    // 如果相等，令p指针指向该关键字，并返回查找成功信息
    *p = T;
    return TRUE;
  } else if (key < T->data) {
    // 如果key值比T根结点的值小，则查找其左子树；反之，查找其右子树
    return SearchBST(T->lchild, key, T, p);
  } else {
    return SearchBST(T->rchild, key, T, p);
  }
}
int InsertBST(BiTree *T, ElemType e) {
  BiTree p = NULL;
  // 如果查找不成功，需做插入操作
  if (!SearchBST((*T), e, NULL, &p)) {
    // 初始化插入结点
    BiTree s = (BiTree)malloc(sizeof(BiTNode));
    s->data = e;
    s->lchild = s->rchild = NULL;
    // 如果p为NULL，说明该二叉排序树为空树，此时插入的结点为整棵树的根结点
    if (!p) {
      *T = s;
    } else if (e < p->data) {
      // 如果p不为NULL，则p指向的为查找失败的最后一个叶子结点，只需要通过比较p和e的值确定s到底是p的左孩子还是右孩子
      p->lchild = s;
    } else {
      p->rchild = s;
    }
    return TRUE;
  }
  // 如果查找成功，不需要做插入操作，插入失败
  return FALSE;
}
// 删除函数
int Delete(BiTree *p) {
  BiTree q, s;
  // 情况1，结点p本身为叶子结点，直接删除即可
  if (!(*p)->lchild && !(*p)->rchild) {
    *p = NULL;
  } else if (
      !(*p)->lchild) {  // 左子树为空，只需用结点p的右子树根结点代替结点p即可；
    q = *p;
    *p = (*p)->rchild;
    free(q);
  } else if (
      !(*p)->rchild) {  // 右子树为空，只需用结点p的左子树根结点代替结点p即可；
    q = *p;
    *p =
        (*p)->lchild;  // 这里不是指针*p指向左子树，而是将左子树存储的结点的地址赋值给指针变量 p
    free(q);
  } else {  // 左右子树均不为空，采用第2种方式
    q = *p;
    s = (*p)->lchild;
    // 遍历，找到结点p的直接前驱
    while (s->rchild) {
      q = s;
      s = s->rchild;
    }
    // 直接改变结点p的值
    (*p)->data = s->data;
    // 判断结点p的左子树s是否有右子树，分为两种情况讨论
    if (q != *p) {
      // 若有，则在删除直接前驱结点的同时，令前驱的左孩子结点改为q指向结点的孩子结点
      q->rchild = s->lchild;
    } else {
      q->lchild = s->lchild;  // 否则，直接将左子树上移即可
    }
    free(s);
  }
  return TRUE;
}
int DeleteBST(BiTree *T, int key) {
  if (!(*T)) {  // 不存在关键字等于key的数据元素
    return FALSE;
  } else {
    if (key == (*T)->data) {
      Delete(T);
      return TRUE;
    } else if (key < (*T)->data) {
      // 使用递归的方式
      return DeleteBST(&(*T)->lchild, key);
    } else {
      return DeleteBST(&(*T)->rchild, key);
    }
  }
}
void order(BiTree t) {  // 中序输出
  if (t == NULL) {
    return;
  }
  order(t->lchild);
  printf("%d ", t->data);
  order(t->rchild);
}

// 使用二叉排序树在查找表中做查找操作的时间复杂度同建立的二叉树本身的结构有关。即使查找表中各数据元素完全相同，但是不同的排列顺序，构建出的二叉排序树大不相同。
// 使用二叉排序树实现动态查找操作的过程，实际上就是从二叉排序树的根结点到查找元素结点的过程，所以时间复杂度同被查找元素所在的树的深度（层次数）有关。
// 为了弥补二叉排序树构造时影响算法效率的因素(每个节点都只有一个孩子)，需要对二叉排序树做“平衡化”处理，使其成为一棵平衡二叉树。

int main() {
  int i;
  int a[5] = {3, 4, 2, 5, 9};
  BiTree T = NULL;
  for (i = 0; i < 5; i++) {
    InsertBST(&T, a[i]);
  }
  printf("中序遍历二叉排序树：\n");
  order(T);
  printf("\n");
  printf("删除3后，中序遍历二叉排序树：\n");
  DeleteBST(&T, 3);
  order(T);
  printf("\n");

  // 中序遍历二叉排序树：
  // 2 3 4 5 9
  // 删除3后，中序遍历二叉排序树：
  // 2 4 5 9

  return 0;
}