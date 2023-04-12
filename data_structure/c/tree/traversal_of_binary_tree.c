#include <stdio.h>
#include <stdlib.h>

#define TElemType int
typedef struct BiTNode {
  TElemType data;                   // 数据域
  struct BiTNode *lchild, *rchild;  // 左右孩子指针
} BiTNode, *BiTree;

void CreateBiTree(BiTree *T);
void displayElem(BiTNode *elem);

int top = -1;  // top变量时刻表示栈顶元素所在位置
// 前序遍历使用的进栈函数
void push(BiTNode **a, BiTNode *elem) { a[++top] = elem; }
// 弹栈函数
void pop() {
  if (top == -1) {
    return;
  }
  top--;
}
// 拿到栈顶元素
BiTNode *getTop(BiTNode **a) { return a[top]; }

// 遍历二叉树可以算作是对树存储结构做的最多的操作，既是重点，也是难点。

//       1
//     2   3
//    4 5 6 7

// 层次遍历：通过对树中各层的节点从左到右依次遍历，即可实现对整棵二叉树的遍历。
// 1 2 3 4 5 6 7
// 整个遍历过程只经过各个节点一次，因此在层次遍历过程，
// 每经过一个节点，都必须立刻访问该节点，否则错失良机，后续无法再对其访问。

// 普通遍历：还有一种更普通的遍历二叉树的思想，即按照从上到下，从左到右的顺序遍历整棵二叉树。
// 而普通遍历方式则不同，整个遍历二叉树的过程中，
// 每个节点都被经过了3次（虽然叶子节点看似只经过了2次，但实际上可以看做是3次）。
// 这也就引出了以下3种遍历二叉树的算法：
// 1.先序遍历，每遇到一个节点，先访问，然后再遍历其左右子树；
//   1 2 4 5 3 6 7
// 2.中序遍历，第一次经过时不访问，等遍历完左子树之后再访问，然后遍历右子树；
//   4 2 5 1 6 3 7
// 3.后序遍历，第一次和第二次经过时都不访问，等遍历完该节点的左右子树之后，最后访问该节点。
//   4 5 2 6 7 3 1

// 二叉树先序遍历的实现思想是：
// 1.访问根节点；
// 2.访问当前节点的左子树；
// 3.若当前节点无左子树，则访问当前节点的右子树。
// 先序遍历递归实现
void PreOrderTraverse1(BiTree T) {
  if (T) {
    displayElem(T);                // 调用操作结点数据的函数方法
    PreOrderTraverse1(T->lchild);  // 访问该结点的左孩子
    PreOrderTraverse1(T->rchild);  // 访问该结点的右孩子
  }
  // 如果结点为空，返回上一层
  return;
}
// 而递归的底层实现依靠的是栈存储结构，
// 因此，二叉树的先序遍历既可以直接采用递归思想实现，也可以使用栈的存储结构模拟递归的思想实现。
// 先序遍历非递归算法
void PreOrderTraverse2(BiTree Tree) {
  BiTNode *a[20];  // 定义一个顺序栈
  BiTNode *p;      // 临时指针
  push(a, Tree);   // 根结点进栈
  while (top != -1) {
    p = getTop(a);  // 取栈顶元素
    pop();          // 弹栈
    while (p) {
      displayElem(p);  // 调用结点的操作函数
      // 如果该结点有右孩子，右孩子进栈
      if (p->rchild) {
        push(a, p->rchild);
      }
      p = p->lchild;  // 一直指向根结点最后一个左孩子
    }
  }
}

// 二叉树中序遍历的实现思想是：
// 1.访问当前节点的左子树；
// 2.访问根节点；
// 3.访问当前节点的右子树。
// 中序遍历递归实现
void INOrderTraverse(BiTree T) {
  if (T) {
    INOrderTraverse(T->lchild);  // 遍历左孩子
    displayElem(T);              // 调用操作结点数据的函数方法
    INOrderTraverse(T->rchild);  // 遍历右孩子
  }
  // 如果结点为空，返回上一层
  return;
}
// 中序遍历的非递归方式实现思想是：
// 从根结点开始，遍历左孩子同时压栈，当遍历结束，说明当前遍历的结点没有左孩子，
// 从栈中取出来调用操作函数，然后访问该结点的右孩子，继续以上重复性的操作。
void InOrderTraverse1(BiTree Tree) {
  BiTNode *a[20];      // 定义一个顺序栈
  BiTNode *p;          // 临时指针
  push(a, Tree);       // 根结点进栈
  while (top != -1) {  // top!=-1说明栈内不为空，程序继续运行
    while ((p = getTop(a)) && p) {  // 取栈顶元素，且不能为NULL
      push(a, p->lchild);  // 将该结点的左孩子进栈，如果没有左孩子，NULL进栈
    }
    pop();  // 跳出循环，栈顶元素肯定为NULL，将NULL弹栈
    if (top != -1) {
      p = getTop(a);  // 取栈顶元素
      pop();          // 栈顶元素弹栈
      displayElem(p);
      push(a, p->rchild);  // 将p指向的结点的右孩子进栈
    }
  }
}
// 除此之外，还有另一种实现思想：中序遍历过程中，只需将每个结点的左子树压栈即可，右子树不需要压栈。
// 当结点的左子树遍历完成后，只需要以栈顶结点的右孩子为根结点，继续循环遍历即可。
void InOrderTraverse2(BiTree Tree) {
  BiTNode *a[20];  // 定义一个顺序栈
  BiTNode *p;      // 临时指针
  p = Tree;
  // 当p为NULL或者栈为空时，表明树遍历完成
  while (p || top != -1) {
    // 如果p不为NULL，将其压栈并遍历其左子树
    if (p) {
      push(a, p);
      p = p->lchild;
    } else {
      // 如果p==NULL，表明左子树遍历完成，需要遍历上一层结点的右子树
      p = getTop(a);
      pop();
      displayElem(p);
      p = p->rchild;
    }
  }
}

// 二叉树后序遍历的实现思想是：
// 从根节点出发，依次遍历各节点的左右子树，直到当前节点左右子树遍历完成后，才访问该节点元素。
// 后序遍历递归实现
void PostOrderTraverse1(BiTree T) {
  if (T) {
    PostOrderTraverse1(T->lchild);  // 遍历左孩子
    PostOrderTraverse1(T->rchild);  // 遍历右孩子
    displayElem(T);                 // 调用操作结点数据的函数方法
  }
  // 如果结点为空，返回上一层
  return;
}
// 后序遍历是在遍历完当前结点的左右孩子之后，才调用操作函数，
// 所以需要在操作结点进栈时，为每个结点配备一个标志位。
// 当遍历该结点的左孩子时，设置当前结点的标志位为0，进栈；
// 当要遍历该结点的右孩子时，设置当前结点的标志位为1，进栈。
// 这样，当遍历完成，该结点弹栈时，查看该结点的标志位的值：
// 如果是0，表示该结点的右孩子还没有遍历；
// 反之如果是1，说明该结点的左右孩子都遍历完成，可以调用操作函数。
// 后序遍历非递归算法
typedef struct SNode {
  BiTree p;
  int tag;
} SNode;
// 后序遍历使用的进栈函数
void postpush(SNode *a, SNode sdata) { a[++top] = sdata; }
void PostOrderTraverse2(BiTree Tree) {
  SNode a[20];  // 定义一个顺序栈
  BiTNode *p;   // 临时指针
  int tag;
  SNode sdata;
  p = Tree;
  while (p || top != -1) {
    while (p) {
      // 为该结点入栈做准备
      sdata.p = p;
      sdata.tag = 0;       // 由于遍历是左孩子，设置标志位为0
      postpush(a, sdata);  // 压栈
      p = p->lchild;       // 以该结点为根结点，遍历左孩子
    }
    sdata = a[top];  // 取栈顶元素
    pop();           // 栈顶元素弹栈
    p = sdata.p;
    tag = sdata.tag;
    // 如果tag==0，说明该结点还没有遍历它的右孩子
    if (tag == 0) {
      sdata.p = p;
      sdata.tag = 1;
      postpush(a, sdata);  // 更改该结点的标志位，重新压栈
      p = p->rchild;  // 以该结点的右孩子为根结点，重复循环
    } else {
      // 如果取出来的栈顶元素的tag==1，说明此结点左右子树都遍历完了，可以调用操作函数了
      displayElem(p);
      p = NULL;
    }
  }
}

// 按照二叉树中的层次从左到右依次遍历每层中的结点。
// 具体的实现思路是：通过使用队列的数据结构，从树的根结点开始，依次将其左孩子和右孩子入队。
// 而后每次队列中一个结点出队，都将其左孩子和右孩子入队，
// 直到树中所有结点都出队，出队结点的先后顺序就是层次遍历的最终结果。
int front = 0, rear = 0;  // 初始化队头和队尾指针开始时都为0
void EnQueue(BiTree *a, BiTree node) { a[rear++] = node; }  // 入队函数
BiTNode *DeQueue(BiTNode **a) { return a[front++]; }        // 出队函数

int main() {
  BiTree Tree;
  CreateBiTree(&Tree);

  printf("先序遍历: \n");
  PreOrderTraverse1(Tree);  // 1 2 4 5 3 6 7
  printf("\n");
  PreOrderTraverse2(Tree);  // 1 2 4 5 3 6 7
  printf("\n");

  printf("中序遍历算法: \n");
  INOrderTraverse(Tree);  // 4 2 5 1 6 3 7
  printf("\n");
  InOrderTraverse1(Tree);  // 4 2 5 1 6 3 7
  printf("\n");
  InOrderTraverse2(Tree);  // 4 2 5 1 6 3 7
  printf("\n");

  printf("后序遍历: \n");
  PostOrderTraverse1(Tree);  // 4 5 2 6 7 3 1
  printf("\n");
  PostOrderTraverse2(Tree);  // 4 5 2 6 7 3 1
  printf("\n");

  printf("层次遍历: \n");
  BiTNode *p;
  // 采用顺序队列，初始化创建队列数组
  BiTree a[20];
  // 根结点入队
  EnQueue(a, Tree);
  // 当队头和队尾相等时，表示队列为空
  while (front < rear) {
    // 队头结点出队
    p = DeQueue(a);
    displayElem(p);
    // 将队头结点的左右孩子依次入队
    if (p->lchild != NULL) {
      EnQueue(a, p->lchild);
    }
    if (p->rchild != NULL) {
      EnQueue(a, p->rchild);
    }
  }
  printf("\n");  // 1 2 3 4 5 6 7

  return 0;
}

void CreateBiTree(BiTree *T) {
  *T = (BiTNode *)malloc(sizeof(BiTNode));
  (*T)->data = 1;
  (*T)->lchild = (BiTNode *)malloc(sizeof(BiTNode));
  (*T)->rchild = (BiTNode *)malloc(sizeof(BiTNode));

  (*T)->lchild->data = 2;
  (*T)->lchild->lchild = (BiTNode *)malloc(sizeof(BiTNode));
  (*T)->lchild->rchild = (BiTNode *)malloc(sizeof(BiTNode));
  (*T)->lchild->rchild->data = 5;
  (*T)->lchild->rchild->lchild = NULL;
  (*T)->lchild->rchild->rchild = NULL;
  (*T)->rchild->data = 3;
  (*T)->rchild->lchild = (BiTNode *)malloc(sizeof(BiTNode));
  (*T)->rchild->lchild->data = 6;
  (*T)->rchild->lchild->lchild = NULL;
  (*T)->rchild->lchild->rchild = NULL;
  (*T)->rchild->rchild = (BiTNode *)malloc(sizeof(BiTNode));
  (*T)->rchild->rchild->data = 7;
  (*T)->rchild->rchild->lchild = NULL;
  (*T)->rchild->rchild->rchild = NULL;
  (*T)->lchild->lchild->data = 4;
  (*T)->lchild->lchild->lchild = NULL;
  (*T)->lchild->lchild->rchild = NULL;
}

// 模拟操作结点元素的函数，输出结点本身的数值
void displayElem(BiTNode *elem) { printf("%d ", elem->data); }