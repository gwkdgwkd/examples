#include <stdio.h>
#include <stdlib.h>

// 常用的遍历方式有两种：深度优先搜索和广度优先搜索。
// 深度优先搜索算法的实现运用的主要是回溯法，类似于树的先序遍历算法。广度优先搜索算法借助队列的先进先出的特点，类似于树的层次遍历。

// 所谓深度优先搜索，是从图中的一个顶点出发，每次遍历当前访问顶点的临界点，一直到访问的顶点没有未被访问过的临界点为止。然后采用依次回退的方式，查看来的
// 路上每一个顶点是否有其它未被访问的临界点。访问完成后，判断图中的顶点是否已经全部遍历完成，如果没有，以未访问的顶点为起始点，重复上述过程。
// 深度优先搜索是一个不断回溯的过程。
#define MAX_VERtEX_NUM 20  // 顶点的最大个数
#define VRType int         // 表示顶点之间的关系的变量类型
#define InfoType char   // 存储弧或者边额外信息的指针变量类型
#define VertexType int  // 图中顶点的数据类型
typedef enum { false, true } bool;  // 定义bool型常量
bool visited[MAX_VERtEX_NUM];  // 设置全局数组，记录标记顶点是否被访问过
typedef struct {
  VRType adj;      // 对于无权图，用1或0表示是否相邻；对于带权图，直接为权值。
  InfoType *info;  // 弧或边额外含有的信息指针
} ArcCell, AdjMatrix[MAX_VERtEX_NUM][MAX_VERtEX_NUM];
typedef struct {
  VertexType vexs[MAX_VERtEX_NUM];  // 存储图中顶点数据
  AdjMatrix arcs;      // 二维数组，记录顶点之间的关系
  int vexnum, arcnum;  // 记录图的顶点数和弧（边）数
} MGraph;
// 根据顶点本身数据，判断出顶点在二维数组中的位置
int LocateVex(MGraph *G, VertexType v) {
  int i = 0;
  // 遍历一维数组，找到变量v
  for (; i < G->vexnum; i++) {
    if (G->vexs[i] == v) {
      break;
    }
  }
  // 如果找不到，输出提示语句，返回-1
  if (i > G->vexnum) {
    printf("no such vertex.\n");
    return -1;
  }
  return i;
}
// 构造无向图
void CreateDN(MGraph *G) {
  scanf("%d,%d", &(G->vexnum), &(G->arcnum));
  for (int i = 0; i < G->vexnum; i++) {
    scanf("%d", &(G->vexs[i]));
  }
  for (int i = 0; i < G->vexnum; i++) {
    for (int j = 0; j < G->vexnum; j++) {
      G->arcs[i][j].adj = 0;
      G->arcs[i][j].info = NULL;
    }
  }
  for (int i = 0; i < G->arcnum; i++) {
    int v1, v2;
    scanf("%d,%d", &v1, &v2);
    int n = LocateVex(G, v1);
    int m = LocateVex(G, v2);
    if (m == -1 || n == -1) {
      printf("no this vertex\n");
      return;
    }
    G->arcs[n][m].adj = 1;
    G->arcs[m][n].adj = 1;  // 无向图的二阶矩阵沿主对角线对称
  }
}
int FirstAdjVex(MGraph G, int v) {
  // 查找与数组下标为v的顶点之间有边的顶点，返回它在数组中的下标
  for (int i = 0; i < G.vexnum; i++) {
    if (G.arcs[v][i].adj) {
      return i;
    }
  }
  return -1;
}
int NextAdjVex(MGraph G, int v, int w) {
  // 从前一个访问位置w的下一个位置开始，查找之间有边的顶点
  for (int i = w + 1; i < G.vexnum; i++) {
    if (G.arcs[v][i].adj) {
      return i;
    }
  }
  return -1;
}
void visitVex(MGraph G, int v) { printf("%d ", G.vexs[v]); }
void DFS(MGraph G, int v) {
  visited[v] = true;  // 标记为true
  visitVex(G, v);     // 访问第v 个顶点
  // 从该顶点的第一个边开始，一直到最后一个边，对处于边另一端的顶点调用DFS函数
  for (int w = FirstAdjVex(G, v); w >= 0; w = NextAdjVex(G, v, w)) {
    // 如果该顶点的标记位false，证明未被访问，调用深度优先搜索函数
    if (!visited[w]) {
      DFS(G, w);
    }
  }
}
// 深度优先搜索
void DFSTraverse(MGraph G) {  //
  int v;
  // 将用做标记的visit数组初始化为false
  for (v = 0; v < G.vexnum; ++v) {
    visited[v] = false;
  }
  // 对于每个标记为false的顶点调用深度优先搜索函数
  for (v = 0; v < G.vexnum; v++) {
    // 如果该顶点的标记位为false，则调用深度优先搜索函数
    if (!visited[v]) {
      DFS(G, v);
    }
  }
}

// 广度优先搜索类似于树的层次遍历。从图中的某一顶点出发，遍历每一个顶点时，依次遍历其所有的邻接点，然后再从这些邻接点出发，同样依次访问它们的邻接点。
// 按照此过程，直到图中所有被访问过的顶点的邻接点都被访问到。
// 最后还需要做的操作就是查看图中是否存在尚未被访问的顶点，若有，则以该顶点为起始点，重复上述遍历的过程。
typedef struct Queue {
  VertexType data;
  struct Queue *next;
} Queue;
// 初始化队列
void InitQueue(Queue **Q) {
  (*Q) = (Queue *)malloc(sizeof(Queue));
  (*Q)->next = NULL;
}
// 顶点元素v进队列
void EnQueue(Queue **Q, VertexType v) {
  Queue *element = (Queue *)malloc(sizeof(Queue));
  element->data = v;
  element->next = NULL;
  Queue *temp = (*Q);
  while (temp->next != NULL) {
    temp = temp->next;
  }
  temp->next = element;
}
// 队头元素出队列
void DeQueue(Queue **Q, int *u) {
  (*u) = (*Q)->next->data;
  (*Q)->next = (*Q)->next->next;
}
// 判断队列是否为空
bool QueueEmpty(Queue *Q) {
  if (Q->next == NULL) {
    return true;
  }
  return false;
}
// 广度优先搜索
void BFSTraverse(MGraph G) {  //
  int v;
  // 将用做标记的visit数组初始化为false
  for (v = 0; v < G.vexnum; ++v) {
    visited[v] = false;
  }
  // 对于每个标记为false的顶点调用深度优先搜索函数
  Queue *Q;
  InitQueue(&Q);
  for (v = 0; v < G.vexnum; v++) {
    if (!visited[v]) {
      visited[v] = true;
      visitVex(G, v);
      EnQueue(&Q, G.vexs[v]);
      while (!QueueEmpty(Q)) {
        int u;
        DeQueue(&Q, &u);
        u = LocateVex(&G, u);
        for (int w = FirstAdjVex(G, u); w >= 0; w = NextAdjVex(G, u, w)) {
          if (!visited[w]) {
            visited[w] = true;
            visitVex(G, w);
            EnQueue(&Q, G.vexs[w]);
          }
        }
      }
    }
  }
}

// 在对无向图进行遍历的时候，遍历过程中所经历过的图中的顶点和边的组合，就是图的生成树或者生成森林。
// 由深度优先搜索得到的树为深度优先生成树。同理，广度优先搜索生成的树为广度优先生成树。

// 非连通图的生成森林
// 非连通图在进行遍历时，实则是对非连通图中每个连通分量分别进行遍历，在遍历过程经过的每个顶点和边，就构成了每个连通分量的生成树。
// 非连通图中，多个连通分量构成的多个生成树为非连通图的生成森林。

// 非连通图在遍历生成森林时，可以采用孩子兄弟表示法将森林转化为一整棵二叉树进行存储。
// 孩子兄弟表示法的链表结点结构
typedef struct CSNode {
  VertexType data;
  struct CSNode *lchild;       // 孩子结点
  struct CSNode *nextsibling;  // 兄弟结点
} * CSTree, CSNode;
void DFSTree(MGraph G, int v, CSTree *T) {
  // 将正在访问的该顶点的标志位设为true
  visited[v] = true;
  bool first = true;
  CSTree q = NULL;
  // 依次遍历该顶点的所有邻接点
  for (int w = FirstAdjVex(G, v); w >= 0; w = NextAdjVex(G, v, w)) {
    // 如果该临界点标志位为false，说明还未访问
    if (!visited[w]) {
      // 为该邻接点初始化为结点
      CSTree p = (CSTree)malloc(sizeof(CSNode));
      p->data = G.vexs[w];
      p->lchild = NULL;
      p->nextsibling = NULL;
      // 该结点的第一个邻接点作为孩子结点，其它邻接点作为孩子结点的兄弟结点
      if (first) {
        (*T)->lchild = p;
        first = false;
      }
      // 否则，为兄弟结点
      else {
        q->nextsibling = p;
      }
      q = p;
      // 以当前访问的顶点为树根，继续访问其邻接点
      DFSTree(G, w, &q);
    }
  }
}
// 深度优先搜索生成森林并转化为二叉树
void DFSForest(MGraph G, CSTree *T) {
  (*T) = NULL;
  // 每个顶点的标记为初始化为false
  for (int v = 0; v < G.vexnum; v++) {
    visited[v] = false;
  }
  CSTree q = NULL;
  // 遍历每个顶点作为初始点，建立深度优先生成树
  for (int v = 0; v < G.vexnum; v++) {
    // 如果该顶点的标记位为false，证明未访问过
    if (!(visited[v])) {
      // 新建一个结点，表示该顶点
      CSTree p = (CSTree)malloc(sizeof(CSNode));
      p->data = G.vexs[v];
      p->lchild = NULL;
      p->nextsibling = NULL;
      // 如果树未空，则该顶点作为树的树根
      if (!(*T)) {
        (*T) = p;

      }
      // 该顶点作为树根的兄弟结点
      else {
        q->nextsibling = p;
      }
      // 每次都要把q指针指向新的结点，为下次添加结点做铺垫
      q = p;
      // 以该结点为起始点，构建深度优先生成树
      DFSTree(G, v, &p);
    }
  }
}
// 前序遍历二叉树
void PreOrderTraverse(CSTree T) {
  if (T) {
    printf("%d ", T->data);
    PreOrderTraverse(T->lchild);
    PreOrderTraverse(T->nextsibling);
  }
  return;
}

// 非连通图采用广度优先搜索算法进行遍历时，经过的顶点以及边的集合为该图的广度优先生成森林。
typedef struct QueueTree {
  CSTree data;  // 队列中存放的为树结点
  struct QueueTree *next;
} QueueTree;
// 初始化队列
void InitQueueTree(QueueTree **Q) {
  (*Q) = (QueueTree *)malloc(sizeof(QueueTree));
  (*Q)->next = NULL;
}
// 结点v进队列
void EnQueueTree(QueueTree **Q, CSTree T) {
  QueueTree *element = (QueueTree *)malloc(sizeof(QueueTree));
  element->data = T;
  element->next = NULL;

  QueueTree *temp = (*Q);
  while (temp->next != NULL) {
    temp = temp->next;
  }
  temp->next = element;
}
// 队头元素出队列
void DeQueueTree(QueueTree **Q, CSTree *u) {
  (*u) = (*Q)->next->data;
  (*Q)->next = (*Q)->next->next;
}
// 判断队列是否为空
bool QueueTreeEmpty(QueueTree *Q) {
  if (Q->next == NULL) {
    return true;
  }
  return false;
}
void BFSTree(MGraph G, int v, CSTree *T) {
  CSTree q = NULL;
  QueueTree *Q;
  InitQueueTree(&Q);
  // 根结点入队
  EnQueueTree(&Q, (*T));
  // 当队列为空时，证明遍历完成
  while (!QueueTreeEmpty(Q)) {
    bool first = true;
    // 队列首个结点出队
    DeQueueTree(&Q, &q);
    // 判断结点中的数据在数组中的具体位置
    int v = LocateVex(&G, q->data);
    // 已经访问过的更改其标志位
    visited[v] = true;
    // 遍历以出队结点为起始点的所有邻接点
    for (int w = FirstAdjVex(G, v); w >= 0; w = NextAdjVex(G, v, w)) {
      //标志位为false，证明未遍历过
      if (!visited[w]) {
        // 新建一个结点 p，存放当前遍历的顶点
        CSTree p = (CSTree)malloc(sizeof(CSNode));
        p->data = G.vexs[w];
        p->lchild = NULL;
        p->nextsibling = NULL;
        // 当前结点入队
        EnQueueTree(&Q, p);
        // 更改标志位
        visited[w] = true;
        // 如果是出队顶点的第一个邻接点，设置p结点为其左孩子
        if (first) {
          q->lchild = p;
          first = false;
        }
        // 否则设置其为兄弟结点
        else {
          q->nextsibling = p;
        }
        q = p;
      }
    }
  }
}
// 广度优先搜索生成森林并转化为二叉树
void BFSForest(MGraph G, CSTree *T) {
  (*T) = NULL;
  // 每个顶点的标记为初始化为false
  for (int v = 0; v < G.vexnum; v++) {
    visited[v] = false;
  }
  CSTree q = NULL;
  // 遍历图中所有的顶点
  for (int v = 0; v < G.vexnum; v++) {
    // 如果该顶点的标记位为false，证明未访问过
    if (!(visited[v])) {
      // 新建一个结点，表示该顶点
      CSTree p = (CSTree)malloc(sizeof(CSNode));
      p->data = G.vexs[v];
      p->lchild = NULL;
      p->nextsibling = NULL;
      // 如果树未空，则该顶点作为树的树根
      if (!(*T)) {
        (*T) = p;
      }
      // 该顶点作为树根的兄弟结点
      else {
        q->nextsibling = p;
      }
      // 每次都要把q指针指向新的结点，为下次添加结点做铺垫
      q = p;
      // 以该结点为起始点，构建广度优先生成树
      BFSTree(G, v, &p);
    }
  }
}

int main() {
  MGraph G;        // 建立一个图的变量
  CreateDN(&G);    // 初始化图
  DFSTraverse(G);  // 深度优先搜索图
  printf("\n");

  BFSTraverse(G);  // 广度优先搜索图
  printf("\n");

  // 8,9
  // 1
  // 2
  // 3
  // 4
  // 5
  // 6
  // 7
  // 8
  // 1,2
  // 2,4
  // 2,5
  // 4,8
  // 5,8
  // 1,3
  // 3,6
  // 6,7
  // 7,3
  // 1 2 4 8 5 3 6 7
  // 1 2 3 4 5 6 7 8

  CSTree T;
  DFSForest(G, &T);
  PreOrderTraverse(T);
  printf("\n");

  CSTree B;
  BFSForest(G, &B);
  PreOrderTraverse(B);
  printf("\n");

  // 13,13
  // 1
  // 2
  // 3
  // 4
  // 5
  // 6
  // 7
  // 8
  // 9
  // 10
  // 11
  // 12
  // 13
  // 1,2
  // 1,3
  // 1,6
  // 1,12
  // 2,13
  // 4,5
  // 7,8
  // 7,10
  // 7,9
  // 8,10
  // 11,12
  // 11,13
  // 12,13
  // 1 2 13 11 12 3 6 4 5 7 8 10 9
  // 1 2 3 6 12 13 11 4 5 7 8 9 10
  // 1 2 13 11 12 3 6 4 5 7 8 10 9
  // 1 2 13 3 6 12 11 4 5 7 8 9 10

  return 0;
}