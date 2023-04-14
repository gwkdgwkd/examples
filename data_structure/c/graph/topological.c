#include <stdio.h>
#include <stdlib.h>

// 在无向图中，如果任意两个顶点之间含有不止一条通路，这个图就被称为重连通图。
// 在重连通图中，在删除某个顶点及该顶点相关的边后，图中各顶点之间的连通性也不会被破坏。
// 在一个无向图中，如果删除某个顶点及其相关联的边后，原来的图被分割为两个及以上的连通分量，
// 则称该顶点为无向图中的一个关节点或者割点。
// 重连通图其实就是没有关节点的连通图。
// 在重连通图中，只删除一个顶点及其相关联的边，肯定不会破坏其连通性。
// 如果一直删除顶点，直到删除K个顶点及其关联的边后，图的连通性才遭到破坏，则称此重连通图的连通度为K。

// 判断重连通图的方法
// 对于任意一个连通图来说，都可以通过深度优先搜索算法获得一棵深度优先生成树。
// 图中有，但是遍历时没有用到的边，简称回边。
// 在深度优先生成树中，图中的关节点有两种特性：
// 1.首先判断整棵树的树根结点，如果树根有两条或者两条以上的子树，则该顶点肯定是关节点。
//   因为一旦树根丢失，生成树就会变成森林。
// 2.然后判断生成树中的每个非叶子结点，
//   以该结点为根结点的每棵子树中如果有结点的回边与此非叶子结点的祖宗结点相关联，
//   那么此非叶子结点就不是关节点；反之，就是关节点。
//   注意：必须是和该非叶子结点的祖宗结点（不包括结点本身）相关联，才说明此结点不是关节点。
// 所以，判断一个图是否是重连通图，也可以转变为：
// 判断图中是否有关节点，如果没有关节点，证明此图为重连通图；反之则不是。

// 拓扑排序指的是将有向无环图（又称DAG图）中的顶点按照图中指定的先后顺序进行排序。
//       v2
//     ↗   ↘            ／▔▔↘
//   v1     v4      v1→v2→v3→v4
//     ↘   ↗         ＼__↗
//      v3
// 两个图形的区别是：左图中的V2和V3之间没有明确的前后顺序；而右图中任意两个顶点之间都有前后顺序。
// 左图中顶点之间的关系被称为偏序关系；右图中顶点之间的关系被称为全序关系。
// 在有向无环图中，弧的方向代表着顶点之间的先后次序，例如从V1指向V2的弧表示在进行排序时V1在前，V2在后。
// 全序是偏序的一种特殊情况。
// 对于任意一个有向无环图来说，通过拓扑排序得到的序列首先一定是偏序，
// 如果任意两个顶点都具有前后顺序，那么此序列是全序。

// 拓扑排序算法的实现过程
// 对有向无环图进行拓扑排序，只需要遵循两个原则：
// 1.在图中选择一个没有前驱的顶点V；
// 2.从图中删除顶点V和所有以该顶点为尾的弧。
// 用来表示某种活动间的优先关系的有向图简称为AOV网。
// 如果顶点之间只是具有偏序关系，那么拓扑排序的结果肯定不唯一；
// 如果顶点之间是全序关系，那么拓扑排序得到的序列唯一。
// 在编写程序解决拓扑排序的问题时，大致思路为：首先通过邻接表将AOV网进行存储，由于拓扑排序的整个过程中，
// 都是以顶点的入度为依据进行排序，所以需要根据建立的邻接表统计出各顶点的入度。
// 在得到各顶点的入度后，首先找到入度为0的顶点作为拓扑排序的起始点，然后查找以该顶点为起始点的所有顶点，
// 如果入度为1，说明如果删除前一个顶点后，该顶点的入度为0，为拓扑排序的下一个对象。

#define MAX_VERTEX_NUM 20  // 最大顶点个数
#define VertexType int     // 顶点数据的类型
typedef enum { false, true } bool;
typedef struct ArcNode {
  int adjvex;               // 邻接点在数组中的位置下标
  struct ArcNode *nextarc;  // 指向下一个邻接点的指针
} ArcNode;
typedef struct VNode {
  VertexType data;                 // 顶点的数据域
  ArcNode *firstarc;               // 指向邻接点的指针
} VNode, AdjList[MAX_VERTEX_NUM];  // 存储各链表头结点的数组
typedef struct {
  AdjList vertices;    // 图中顶点及各邻接点数组
  int vexnum, arcnum;  // 记录图中顶点数（<100）和边或弧数
} ALGraph;
// 找到顶点对应在邻接表数组中的位置下标
int LocateVex(ALGraph G, VertexType u) {
  for (int i = 0; i < G.vexnum; i++) {
    if (G.vertices[i].data == u) {
      return i;
    }
  }
  return -1;
}
// 创建AOV网，构建邻接表
void CreateAOV(ALGraph **G) {
  *G = (ALGraph *)malloc(sizeof(ALGraph));

  scanf("%d,%d", &((*G)->vexnum), &((*G)->arcnum));
  for (int i = 0; i < (*G)->vexnum; i++) {
    scanf("%d", &((*G)->vertices[i].data));
    (*G)->vertices[i].firstarc = NULL;
  }
  VertexType initial, end;
  for (int i = 0; i < (*G)->arcnum; i++) {
    scanf("%d,%d", &initial, &end);

    ArcNode *p = (ArcNode *)malloc(sizeof(ArcNode));
    p->adjvex = LocateVex(*(*G), end);
    p->nextarc = NULL;

    int locate = LocateVex(*(*G), initial);
    p->nextarc = (*G)->vertices[locate].firstarc;
    (*G)->vertices[locate].firstarc = p;
  }
}
// 结构体定义栈结构
typedef struct stack {
  VertexType data;
  struct stack *next;
} stack;
// 初始化栈结构
void initStack(stack **S) {
  (*S) = (stack *)malloc(sizeof(stack));
  (*S)->next = NULL;
}
// 判断链表是否为空
bool StackEmpty(stack S) {
  if (S.next == NULL) {
    return true;
  }
  return false;
}
// 进栈，以头插法将新结点插入到链表中
void push(stack *S, VertexType u) {
  stack *p = (stack *)malloc(sizeof(stack));
  p->data = u;
  p->next = NULL;
  p->next = S->next;
  S->next = p;
}
// 弹栈函数，删除链表首元结点的同时，释放该空间，并将该结点中的数据域通过地址传值给变量i;
void pop(stack *S, VertexType *i) {
  stack *p = S->next;
  *i = p->data;
  S->next = S->next->next;
  free(p);
}
// 统计各顶点的入度
void FindInDegree(ALGraph G, int indegree[]) {
  // 初始化数组，默认初始值全部为0
  for (int i = 0; i < G.vexnum; i++) {
    indegree[i] = 0;
  }
  // 遍历邻接表，根据各链表中结点的数据域存储的各顶点位置下标，在indegree数组相应位置+1
  for (int i = 0; i < G.vexnum; i++) {
    ArcNode *p = G.vertices[i].firstarc;
    while (p) {
      indegree[p->adjvex]++;
      p = p->nextarc;
    }
  }
}
void TopologicalSort(ALGraph G) {
  int indegree[100];  // 创建记录各顶点入度的数组，数量为G.vexnum
  FindInDegree(G, indegree);  // 统计各顶点的入度
  // 建立栈结构，程序中使用的是链表
  stack *S;
  initStack(&S);
  // 查找度为0的顶点，作为起始点
  for (int i = 0; i < G.vexnum; i++) {
    if (!indegree[i]) {
      push(S, i);
    }
  }
  int count = 0;
  // 当栈为空，说明排序完成
  while (!StackEmpty(*S)) {
    int index;
    // 弹栈，并记录栈中保存的顶点所在邻接表数组中的位置
    pop(S, &index);
    printf("%d", G.vertices[index].data);
    ++count;
    // 依次查找跟该顶点相链接的顶点，如果初始入度为1，当删除前一个顶点后，该顶点入度为0
    for (ArcNode *p = G.vertices[index].firstarc; p; p = p->nextarc) {
      VertexType k = p->adjvex;
      if (!(--indegree[k])) {
        // 顶点入度为0，入栈
        push(S, k);
      }
    }
  }
  // 如果count值小于顶点数量，表明该有向图有环
  if (count < G.vexnum) {
    printf("该图有回路");
    return;
  }
}
int main() {
  ALGraph *G;
  CreateAOV(&G);        // 创建AOV网
  TopologicalSort(*G);  // 进行拓扑排序
  printf("\n");
  // 6,8
  // 1
  // 2
  // 3
  // 4
  // 5
  // 6
  // 1,2
  // 1,4
  // 1,3
  // 3,2
  // 3,5
  // 4,5
  // 6,4
  // 6,5
  // 614325

  return 0;
}