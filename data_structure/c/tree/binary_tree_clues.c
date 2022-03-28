#include <stdio.h>
#include <stdlib.h>

// 二叉树本身是一种非线性结构，采用任何一种遍历二叉树的方法，都可以得到树中所有结点的一个线性序列。
// 在这个序列中，除第一个结点外，每个结点都有自己的直接前趋；
// 除最后一个结点外，每个结点都有一个直接后继。
// 如果算法中多次涉及到对二叉树的遍历，普通的二叉树就需要使用栈结构做重复性的操作。
// 线索二叉树不需要如此，在遍历的同时，使用二叉树中空闲的内存空间记录某些结点的前趋和后继元素的位置（不是全部）。
// 这样在算法后期需要遍历二叉树时，就可以利用保存的结点信息，提高了遍历的效率。
// 使用这种方法构建的二叉树，即为“线索二叉树”。

// 如果在二叉树中想保存每个结点前趋和后继所在的位置信息，最直接的想法就是改变结点的结构，
// 即添加两个指针域，分别指向该结点的前趋和后继。
// 但是这种方式会降低树存储结构的存储密度(数据本身所占的存储空间和整个结点结构所占的存储量之比)。
// 而对于二叉树来讲，其本身还有很多未利用的空间。
// 每一棵二叉树上，很多结点都含有未使用的指向NULL的指针域。
// 除了度为2的结点，度为1的结点，有一个空的指针域；叶子结点两个指针域都为NULL。
// 规律：在有n个结点的二叉链表中必定存在n+1个空指针域。
// 线索二叉树实际上就是使用这些空指针域来存储结点之间前趋和后继关系的一种特殊的二叉树。
// 线索二叉树中，如果结点有左子树，则lchild指针域指向左孩子，否则lchild指针域指向该结点的直接前趋；
// 同样，如果结点有右子树，则rchild指针域指向右孩子，否则rchild指针域指向该结点的直接后继。
// 为了避免指针域指向的结点的意义混淆，需要改变结点本身的结构，增加两个标志域，LTag和RTag为标志域。
// 实际上就是两个布尔类型的变量：
// 1.LTag值为0时，表示lchild指针域指向的是该结点的左孩子；为1时，表示指向的是该结点的直接前趋结点；
// 2.RTag值为0时，表示rchild指针域指向的是该结点的右孩子；为1时，表示指向的是该结点的直接后继结点。
#define TElemType int  // 宏定义，结点中数据域的类型
// 枚举，Link为0，Thread为1
typedef enum PointerTag { Link, Thread } PointerTag;
// 结点结构构造
typedef struct BiThrNode {
  TElemType data;                     // 数据域
  struct BiThrNode *lchild, *rchild;  // 左孩子，右孩子指针域

  PointerTag Ltag, Rtag;  // 标志域，枚举类型
} BiThrNode, *BiThrTree;
// 使用上面的结点结构构成的二叉链表，被称为线索链表；构建的二叉树称为线索二叉树。
// 线索链表中的“线索”，指的是链表中指向结点前趋和后继的指针。
// 二叉树经过某种遍历方法转化为线索二叉树的过程称为线索化。

// 对二叉树进行线索化
// 将二叉树转化为线索二叉树，实质上是在遍历二叉树的过程中，将二叉链表中的空指针改为指向直接前趋或者直接后继的线索。
// 线索化的过程即为在遍历的过程中修改空指针的过程。
// 在遍历过程中，如果当前结点没有左孩子，需要将该结点的lchild指针指向遍历过程中的前一个结点，
// 所以在遍历过程中，设置一个指针（名为pre），时刻指向当前访问结点的前一个结点。
// 中序对二叉树进行线索化
BiThrTree pre = NULL;
void InThreading(BiThrTree p) {
  // 如果当前结点存在
  if (p) {
    InThreading(p->lchild);  // 递归当前结点的左子树，进行线索化
    // 如果当前结点没有左孩子，左标志位设为1，左指针域指向上一结点pre
    if (!p->lchild) {
      p->Ltag = Thread;
      p->lchild = pre;
    }
    // 如果pre没有右孩子，右标志位设为1，右指针域指向当前结点。
    if (!pre->rchild) {
      pre->Rtag = Thread;
      pre->rchild = p;
    }
    pre = p;  // 线索化完左子树后，让pre指针指向当前结点
    InThreading(p->rchild);  // 递归右子树进行线索化
  }
}

// 线索二叉树遍历
// 使用线索二叉树时，会经常遇到一个问题，结点的直接后继直接通过指针域获得，为结点*；
// 而由于结点*的度为2，无法利用指针域指向后继结点，整个链表断掉了。
// 当在遍历过程，遇到这种问题是解决的办法就是：寻找先序、中序、后序遍历的规律，找到下一个结点。
// 1.在先序遍历过程中，如果结点因为有右孩子导致无法找到其后继结点，如果结点有左孩子，则后继结点是其左孩子；
//   否则，就一定是右孩子。
// 2.在中序遍历过程中，结点的后继是遍历其右子树时访问的第一个结点，也就是右子树中位于最左下的结点。
// 3.后序遍历中找后继结点需要分为3种情况：
//   a.如果该结点是二叉树的根，后继结点为空；
//   b.如果该结点是父结点的右孩子（或者是左孩子，但是父结点没有右孩子），后继结点是父结点；
//   c.如果该结点是父结点的左孩子，且父结点有右子树，后继结点为父结点的右子树在后序遍历列出的第一个结点。
//   使用后序遍历建立的线索二叉树，在真正使用过程中遇到链表的断点时，需要访问父结点，
//   所以在初步建立二叉树时，宜采用三叉链表做存储结构。
// 中序遍历线索二叉树
void InOrderThraverse_Thr(BiThrTree p) {
  while (p) {
    // 一直找左孩子，最后一个为中序序列中排第一的
    while (p->Ltag == Link) {
      p = p->lchild;
    }
    printf("%c ", p->data);  // 操作结点数据
    // 当结点右标志位为1时，直接找到其后继结点
    while (p->Rtag == Thread && p->rchild != NULL) {
      p = p->rchild;
      printf("%c ", p->data);
    }
    // 否则，按照中序遍历的规律，找其右子树中最左下的结点，也就是继续循环遍历
    p = p->rchild;
  }
}

// 线索二叉链表可以从第一个结点往后逐个遍历。
// 但是起初由于没有记录中序序列中最后一个结点的位置，所以不能实现从最后一个结点往前逐个遍历。
// 双向线索链表的作用就是可以让线索二叉树从两个方向实现遍历。
// 在线索二叉树的基础上，额外添加一个结点。
// 此结点的作用类似于链表中的头指针，数据域不起作用，只利用两个指针域（由于都是指针，标志域都为0）。
// 左指针域指向二叉树的树根，确保可以正方向对二叉树进行遍历；
// 同时，右指针指向线索二叉树形成的线性序列中的最后一个结点。
// 这样，二叉树中的线索链表就变成了双向线索链表，既可以从第一个结点通过不断地找后继结点进行遍历，
// 也可以从最后一个结点通过不断找前趋结点进行遍历。
// 建立双向线索链表
void InOrderThread_Head(BiThrTree *h, BiThrTree t) {
  // 初始化头结点
  (*h) = (BiThrTree)malloc(sizeof(BiThrNode));
  if ((*h) == NULL) {
    printf("申请内存失败");
    return;
  }
  (*h)->rchild = *h;
  (*h)->Rtag = Link;
  // 如果树本身是空树
  if (!t) {
    (*h)->lchild = *h;
    (*h)->Ltag = Link;
  } else {
    pre = *h;          // pre指向头结点
    (*h)->lchild = t;  // 头结点左孩子设为树根结点
    (*h)->Ltag = Link;
    // 线索化二叉树，pre结点作为全局变量，线索化结束后，pre结点指向中序序列中最后一个结点
    InThreading(t);
    pre->rchild = *h;
    pre->Rtag = Thread;
    (*h)->rchild = pre;
  }
}
// 双向线索二叉树的遍历
// 双向线索二叉树遍历时，如果正向遍历，就从树的根结点开始。
// 整个遍历过程结束的标志是：当从头结点出发，遍历回头结点时，表示遍历结束。
// 中序正向遍历双向线索二叉树
void InOrderThraverse_Thr2(BiThrTree h) {
  BiThrTree p;
  p = h->lchild;  // p指向根结点
  while (p != h) {
    while (p->Ltag == Link)  // 当ltag = 0时循环到中序序列的第一个结点
    {
      p = p->lchild;
    }
    printf("%c ", p->data);  // 显示结点数据，可以更改为其他对结点的操作
    while (p->Rtag == Thread && p->rchild != h) {
      p = p->rchild;
      printf("%c ", p->data);
    }

    p = p->rchild;  // p进入其右子树
  }
}
// 逆向遍历线索二叉树的过程即从头结点的右指针指向的结点出发，逐个寻找直接前趋结点，结束标志同正向遍历一样：
// 中序逆方向遍历线索二叉树
void InOrderThraverse_Thr3(BiThrTree h) {
  BiThrTree p;
  p = h->rchild;
  while (p != h) {
    while (p->Rtag == Link) {
      p = p->rchild;
    }
    printf("%c", p->data);
    //如果lchild为线索，直接使用，输出
    while (p->Ltag == Thread && p->lchild != h) {
      p = p->lchild;
      printf("%c", p->data);
    }
    p = p->lchild;
  }
}

void CreateTree(BiThrTree *tree) {
  char data;
  scanf("%c", &data);
  printf("%c\n", data);
  if (data != '#') {
    if (!((*tree) = (BiThrNode *)malloc(sizeof(BiThrNode)))) {
      printf("申请结点空间失败");
      return;
    } else {
      (*tree)->data = data;  // 采用前序遍历方式初始化二叉树
      (*tree)->Ltag = Link;
      (*tree)->Rtag = Link;
      CreateTree(&((*tree)->lchild));  // 初始化左子树
      CreateTree(&((*tree)->rchild));  // 初始化右子树
    }
  } else {
    *tree = NULL;
  }
}

// 输入前序二叉树:
// 124###35##6##
// 输出中序序列:
// 4 2 1 5 3 6
int main() {
#if 1
  BiThrTree t;
  printf("输入前序二叉树:\n");
  CreateTree(&t);
  InThreading(t);
  printf("输出中序序列:\n");
  InOrderThraverse_Thr(t);
#else
  BiThrTree t;
  BiThrTree h;
  printf("输入前序二叉树:\n");
  CreateTree(&t);
  InOrderThread_Head(&h, t);
  printf("输出中序序列:\n");
  InOrderThraverse_Thr2(h);
// InOrderThraverse_Thr3(h);
#endif

  return 0;
}