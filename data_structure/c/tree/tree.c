#include <stdio.h>
#include <stdlib.h>

// 树结构是一种非线性存储结构，存储的是具有“一对多”关系的数据元素的集合。
// 树型存储结构类似于家族的族谱，各个结点之间也同样可能具有父子、兄弟、表兄弟的关系。

// 树的结点
// 结点：使用树结构存储的每一个数据元素都被称为“结点”。
// 父结点（双亲结点）、子结点和兄弟结点：
// 树根结点（简称“根结点”）：每一个非空树都有且只有一个被称为根的结点。是整棵树的根结点。
// 树根的判断依据为：如果一个结点没有父结点，那么这个结点就是整棵树的根结点。
// 叶子结点：如果结点没有任何子结点，那么此结点称为叶子结点（叶结点）。

// 子树和空树
// 单个结点也是一棵树，只不过根结点就是它本身。
// 空树：如果集合本身为空，那么构成的树就被称为空树。空树中没有结点。
// 在树结构中，对于具有同一个根结点的各个子树，相互之间不能有交集。

// 结点的度和层次
// 对于一个结点，拥有的子树数（结点有多少分支）称为结点的度（Degree）。一棵树的度是树内各结点的度的最大值。
// 结点的层次：从一棵树的树根开始，树根所在层为第一层，根的孩子结点所在的层为第二层，依次类推。一棵树的深度（高度）是树中结点所在的最大的层次。
// 如果两个结点的父结点虽不相同，但是它们的父结点处在同一层次上，那么这两个结点互为堂兄弟。

// 有序树和无序树
// 如果树中结点的子树从左到右看，谁在左边，谁在右边，是有规定的，这棵树称为有序树；反之称为无序树。
// 在有序树中，一个结点最左边的子树称为"第一个孩子"，最右边的称为"最后一个孩子"。

// 森林:由m（m >= 0）个互不相交的树组成的集合被称为森林。

// 树的表示方法
//  用树型图表示
//  以嵌套的集合的形式表示的（集合之间绝不能相交，即图中任意两个圈不能相交）。
//  凹入表示法
//  最常用的表示方法是使用广义表的方式。用广义表表示为：(A,(B(E(K,L),F),C(G),D(H(M),I,J)))

//              R
//            / | \
//           A  B  C
//          / \     |
//         D   E    F
//                / | \
//               G  H  K
// 一棵普通的树，该如何存储呢？通常，存储具有普通树结构数据的方法有3种：
//  双亲表示法；
//  孩子表示法；
//  孩子兄弟表示法；

// 双亲表示法采用顺序表（也就是数组）存储普通树，其实现的核心思想是：顺序存储各个节点的同时，给各节点附加一个记录其父节点位置的变量。
// 注意，根节点没有父节点（父节点又称为双亲节点），因此根节点记录父节点位置的变量通常置为-1。
//  数组下标  data  parent
//     0      R      -1
//     1      A      0
//     2      B      0
//     3      C      0
//     4      D      1
//     5      E      1
//     6      F      3
//     7      G      6
//     8      H      6
//     9      K      6
#define MAX_SIZE 20
typedef char ElemType;  // 宏定义树结构中数据类型
typedef struct Snode {  // 结点结构
  ElemType data;
  int parent;
} PNode;
typedef struct {  // 树结构
  PNode tnode[MAX_SIZE];
  int n;  // 结点个数
} PTree;
PTree InitPNode(PTree tree) {
  int i, j;
  char ch;
  printf("请输出节点个数:\n");
  scanf("%d", &(tree.n));
  printf("请输入结点的值其双亲位于数组中的位置下标:\n");
  for (i = 0; i < tree.n; i++) {
    getchar();
    scanf("%c %d", &ch, &j);
    tree.tnode[i].data = ch;
    tree.tnode[i].parent = j;
  }
  return tree;
}
void FindParent(PTree tree) {
  char a;
  int isfind = 0;
  printf("请输入要查询的结点值:\n");
  getchar();
  scanf("%c", &a);
  for (int i = 0; i < tree.n; i++) {
    if (tree.tnode[i].data == a) {
      isfind = 1;
      int ad = tree.tnode[i].parent;
      printf("%c的父节点为%c,存储位置下标为%d\n", a, tree.tnode[ad].data, ad);
      break;
    }
  }
  if (isfind == 0) {
    printf("树中无此节点\n");
  }
}

// 孩子表示法存储普通树采用的是"顺序表+链表"的组合结构，其存储过程是：从树的根节点开始，使用顺序表依次存储树中各个节点，
// 需要注意的是，与双亲表示法不同，孩子表示法会给各个节点配备一个链表，用于存储各节点的孩子节点位于顺序表中的位置。
// 如果节点没有孩子节点（叶子节点），则该节点的链表为空链表。
// 还可以将双亲表示法和孩子表示法合二为一，既能快速找到指定节点的父节点，又能快速找到指定节点的孩子节点。整合两部分代码即可。
typedef struct CTNode {
  int child;  // 链表中每个结点存储的不是数据本身，而是数据在数组中存储的位置下标
  struct CTNode* next;
} ChildPtr;
typedef struct {
  ElemType data;         // 结点的数据类型
  ChildPtr* firstchild;  // 孩子链表的头指针
} CTBox;
typedef struct {
  CTBox nodes[MAX_SIZE];  // 存储结点的数组
  int n, r;               // 结点数量和树根的位置
} CTree;
// 孩子表示法存储普通树
CTree initTree(CTree tree) {
  printf("输入节点数量：\n");
  scanf("%d", &(tree.n));
  for (int i = 0; i < tree.n; i++) {
    printf("输入第 %d 个节点的值：\n", i + 1);
    getchar();
    scanf("%c", &(tree.nodes[i].data));
    tree.nodes[i].firstchild = (ChildPtr*)malloc(sizeof(ChildPtr));
    tree.nodes[i].firstchild->next = NULL;
    printf("输入节点 %c 的孩子节点数量：\n", tree.nodes[i].data);
    int Num;
    scanf("%d", &Num);
    if (Num != 0) {
      ChildPtr* p = tree.nodes[i].firstchild;
      for (int j = 0; j < Num; j++) {
        ChildPtr* newEle = (ChildPtr*)malloc(sizeof(ChildPtr));
        newEle->next = NULL;
        printf("输入第 %d 个孩子节点在顺序表中的位置", j + 1);
        scanf("%d", &(newEle->child));
        p->next = newEle;
        p = p->next;
      }
    }
  }
  return tree;
}
void findKids(CTree tree, char a) {
  int hasKids = 0;
  for (int i = 0; i < tree.n; i++) {
    if (tree.nodes[i].data == a) {
      ChildPtr* p = tree.nodes[i].firstchild->next;
      while (p) {
        hasKids = 1;
        printf("%c ", tree.nodes[p->child].data);
        p = p->next;
      }
      break;
    }
  }
  if (hasKids == 0) {
    printf("此节点为叶子节点\n");
  }
}

// 树结构中，位于同一层的节点之间互为兄弟节点。
// 孩子兄弟表示法，采用的是链式存储结构，其存储树的实现思想是：从树的根节点开始，依次用链表存储各个节点的孩子节点和兄弟节点。
// 因此，该链表中的节点应包含以下3部分内容：
//  节点的值；
//  指向孩子节点的指针；
//  指向兄弟节点的指针；
typedef struct CSNode {
  ElemType data;
  struct CSNode *firstchild, *nextsibling;
} CSNode, *CSTree;
// 通过孩子兄弟表示法，任意一棵普通树都可以相应转化为一棵二叉树，换句话说，任意一棵普通树都有唯一的一棵二叉树于其对应。
// 因此，孩子兄弟表示法可以作为将普通树转化为二叉树的最有效方法，通常又被称为"二叉树表示法"或"二叉链表表示法"。

int main() {
  int flag = 1;

  if (flag == 0) {
    PTree tree;
    for (int i = 0; i < MAX_SIZE; i++) {
      tree.tnode[i].data = ' ';
      tree.tnode[i].parent = 0;
    }
    tree = InitPNode(tree);
    FindParent(tree);
    // 请输出节点个数:
    // 10
    // 请输入结点的值其双亲位于数组中的位置下标:
    // R -1
    // A 0
    // B 0
    // C 0
    // D 1
    // E 1
    // F 3
    // G 6
    // H 6
    // K 6
    // 请输入要查询的结点值:
    // C
    // C的父节点为 R,存储位置下标为0
  } else if (flag == 1) {
    CTree tree;
    for (int i = 0; i < MAX_SIZE; i++) {
      tree.nodes[i].firstchild = NULL;
    }
    tree = initTree(tree);
    // 默认数根节点位于数组notes[0]处
    tree.r = 0;
    printf("找出节点 F 的所有孩子节点：");
    findKids(tree, 'F');
    printf("\n");
    // 输入节点数量：
    // 10
    // 输入第 1 个节点的值：
    // R
    // 输入节点 R 的孩子节点数量：
    // 3
    // 输入第 1 个孩子节点在顺序表中的位置1
    // 输入第 2 个孩子节点在顺序表中的位置2
    // 输入第 3 个孩子节点在顺序表中的位置3
    // 输入第 2 个节点的值：
    // A
    // 输入节点 A 的孩子节点数量：
    // 2
    // 输入第 1 个孩子节点在顺序表中的位置4
    // 输入第 2 个孩子节点在顺序表中的位置5
    // 输入第 3 个节点的值：
    // B
    // 输入节点 B 的孩子节点数量：
    // 0
    // 输入第 4 个节点的值：
    // C
    // 输入节点 C 的孩子节点数量：
    // 1
    // 输入第 1 个孩子节点在顺序表中的位置6
    // 输入第 5 个节点的值：
    // D
    // 输入节点 D 的孩子节点数量：
    // 0
    // 输入第 6 个节点的值：
    // E
    // 输入节点 E 的孩子节点数量：
    // 0
    // 输入第 7 个节点的值：
    // F
    // 输入节点 F 的孩子节点数量：
    // 3
    // 输入第 1 个孩子节点在顺序表中的位置7
    // 输入第 2 个孩子节点在顺序表中的位置8
    // 输入第 3 个孩子节点在顺序表中的位置9
    // 输入第 8 个节点的值：
    // G
    // 输入节点 G 的孩子节点数量：
    // 0
    // 输入第 9 个节点的值：
    // H
    // 输入节点 H 的孩子节点数量：
    // 0
    // 输入第 10 个节点的值：
    // K
    // 输入节点 K 的孩子节点数量：
    // 0
    // 找出节点 F 的所有孩子节点：G H K
  }

  return 0;
}