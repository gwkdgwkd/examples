#include <stdio.h>
#include <stdlib.h>

// 对于含有n个顶点的连通图来说可能包含有多种生成树。
// 给定一个带有权值的连通图（连通网），如何从众多的生成树中筛选出权值总和最小的生成树，即为该图的最小生成树。
// 给定一个连通网，求最小生成树的方法有：普里姆（Prim）算法和克鲁斯卡尔（Kruskal）算法。

// 普里姆算法
// 普里姆算法在找最小生成树时，将顶点分为两类，一类是在查找的过程中已经包含在树中的（假设为A类），剩下的是另一类（假设为B类）。
// 对于给定的连通网，起始状态全部顶点都归为B类。在找最小生成树时，选定任意一个顶点作为起始点，并将之从B类移至A类；然后找出B类中到A类中的顶点之间权值最小的顶点，
// 将之从B类移至A类，如此重复，直到B类中没有顶点为止。所走过的顶点和边就是该连通图的最小生成树。
// 普里姆算法的运行效率只与连通网中包含的顶点数相关，而和网所含的边数无关。所以普里姆算法适合于解决边稠密的网，该算法运行的时间复杂度为：O(n2)。
#define VertexType int
#define VRType int
#define MAX_VERtEX_NUM 20
#define InfoType char
#define INFINITY 65535
typedef struct {
  VRType adj;      // 对于无权图，用1或0表示是否相邻；对于带权图，直接为权值。
  InfoType *info;  // 弧额外含有的信息指针
} ArcCell, AdjMatrix[MAX_VERtEX_NUM][MAX_VERtEX_NUM];
typedef struct {
  VertexType vexs[MAX_VERtEX_NUM];  // 存储图中顶点数据
  AdjMatrix arcs;      // 二维数组，记录顶点之间的关系
  int vexnum, arcnum;  // 记录图的顶点数和弧（边）数
} MGraph;
// 根据顶点本身数据，判断出顶点在二维数组中的位置
int LocateVex(MGraph G, VertexType v) {
  int i = 0;
  // 遍历一维数组，找到变量v
  for (; i < G.vexnum; i++) {
    if (G.vexs[i] == v) {
      return i;
    }
  }
  return -1;
}
// 构造无向网
void CreateUDN(MGraph *G) {
  scanf("%d,%d", &(G->vexnum), &(G->arcnum));
  for (int i = 0; i < G->vexnum; i++) {
    scanf("%d", &(G->vexs[i]));
  }
  for (int i = 0; i < G->vexnum; i++) {
    for (int j = 0; j < G->vexnum; j++) {
      G->arcs[i][j].adj = INFINITY;
      G->arcs[i][j].info = NULL;
    }
  }
  for (int i = 0; i < G->arcnum; i++) {
    int v1, v2, w;
    scanf("%d,%d,%d", &v1, &v2, &w);
    int m = LocateVex(*G, v1);
    int n = LocateVex(*G, v2);
    if (m == -1 || n == -1) {
      printf("no this vertex\n");
      return;
    }
    G->arcs[n][m].adj = w;
    G->arcs[m][n].adj = w;
  }
}
// 辅助数组，用于每次筛选出权值最小的边的邻接点
typedef struct {
  VertexType adjvex;  // 记录权值最小的边的起始点
  VRType lowcost;     // 记录该边的权值
} closedge[MAX_VERtEX_NUM];
closedge theclose;  // 创建一个全局数组，因为每个函数中都会使用到
// 在辅助数组中找出权值最小的边的数组下标，就可以间接找到此边的终点顶点。
int minimun(MGraph G, closedge close) {
  int min = INFINITY;
  int min_i = -1;
  for (int i = 0; i < G.vexnum; i++) {
    // 权值为0，说明顶点已经归入最小生成树中；然后每次和min变量进行比较，最后找出最小的。
    if (close[i].lowcost > 0 && close[i].lowcost < min) {
      min = close[i].lowcost;
      min_i = i;
    }
  }
  // 返回最小权值所在的数组下标
  return min_i;
}
// 普里姆算法函数，G为无向网，u为在网中选择的任意顶点作为起始点
void miniSpanTreePrim(MGraph G, VertexType u) {
  // 找到该起始点在顶点数组中的位置下标
  int k = LocateVex(G, u);
  // 首先将与该起始点相关的所有边的信息：边的起始点和权值，存入辅助数组中相应的位置，例如（1，2）边，adjvex为0，lowcost为6，存入theclose[1]中，辅助数组的下标表示该边的顶点2
  for (int i = 0; i < G.vexnum; i++) {
    if (i != k) {
      theclose[i].adjvex = k;
      theclose[i].lowcost = G.arcs[k][i].adj;
    }
  }
  // 由于起始点已经归为最小生成树，所以辅助数组对应位置的权值为0，这样，遍历时就不会被选中
  theclose[k].lowcost = 0;
  // 选择下一个点，并更新辅助数组中的信息
  for (int i = 1; i < G.vexnum; i++) {
    // 找出权值最小的边所在数组下标
    k = minimun(G, theclose);
    // 输出选择的路径
    printf("v%d v%d\n", G.vexs[theclose[k].adjvex], G.vexs[k]);
    // 归入最小生成树的顶点的辅助数组中的权值设为0
    theclose[k].lowcost = 0;
    // 信息辅助数组中存储的信息，由于此时树中新加入了一个顶点，需要判断，由此顶点出发，到达其它各顶点的权值是否比之前记录的权值还要小，如果还小，则更新
    for (int j = 0; j < G.vexnum; j++) {
      if (G.arcs[k][j].adj < theclose[j].lowcost) {
        theclose[j].adjvex = k;
        theclose[j].lowcost = G.arcs[k][j].adj;
      }
    }
  }
  printf("\n");
}

// 克鲁斯卡尔算法(Kruskal算法)求最小生成树
// 最小生成树之普里姆算法。该算法从顶点的角度为出发点，时间复杂度为O(n2)，更适合与解决边的绸密度更高的连通网。
// 克鲁斯卡尔算法，从边的角度求网的最小生成树，时间复杂度为O(eloge)。和普里姆算法恰恰相反，更适合于求边稀疏的网的最小生成树。
// 对于任意一个连通网的最小生成树来说，在要求总的权值最小的情况下，最直接的想法就是将连通网中的所有边按照权值大小进行升序排序，从小到大依次选择。
// 由于最小生成树本身是一棵生成树，所以需要时刻满足以下两点：
//  生成树中任意顶点之间有且仅有一条通路，也就是说，生成树中不能存在回路；
//  对于具有n个顶点的连通网，其生成树中只能有n-1条边，这n-1条边连通着n个顶点。
// 连接n个顶点在不产生回路的情况下，只需要n-1条边。
// 所以克鲁斯卡尔算法的具体思路是：将所有边按照权值的大小进行升序排序，然后从小到大一一判断，条件为：如果这个边不会与之前选择的所有边组成回路，
// 就可以作为最小生成树的一部分；反之，舍去。直到具有n个顶点的连通网筛选出来n-1条边为止。筛选出来的边和所有的顶点构成此连通网的最小生成树。
// 判断是否会产生回路的方法为：在初始状态下给每个顶点赋予不同的标记，对于遍历过程的每条边，其都有两个顶点，判断这两个顶点的标记是否一致，如果一致，
// 说明它们本身就处在一棵树中，如果继续连接就会产生回路；如果不一致，说明它们之间还没有任何关系，可以连接。
#define MAX_VERtEX_NUM 20
#define VertexType int
typedef struct edge {
  VertexType initial;
  VertexType end;
  VertexType weight;
} edge[MAX_VERtEX_NUM];
// 定义辅助数组
typedef struct {
  VertexType value;  // 顶点数据
  int sign;          // 每个顶点所属的集合
} assist[MAX_VERtEX_NUM];
assist assists;
// qsort排序函数中使用，使edges结构体中的边按照权值大小升序排序
int cmp(const void *a, const void *b) {
  return ((struct edge *)a)->weight - ((struct edge *)b)->weight;
}
// 初始化连通网
void CreateUDN1(edge *edges, int *vexnum, int *arcnum) {
  printf("输入连通网的边数：\n");
  scanf("%d %d", &(*vexnum), &(*arcnum));
  printf("输入连通网的顶点：\n");
  for (int i = 0; i < (*vexnum); i++) {
    scanf("%d", &(assists[i].value));
    assists[i].sign = i;
  }
  printf("输入各边的起始点和终点及权重：\n");
  for (int i = 0; i < (*arcnum); i++) {
    scanf("%d,%d,%d", &(*edges)[i].initial, &(*edges)[i].end,
          &(*edges)[i].weight);
  }
}
// 在assists数组中找到顶点point对应的位置下标
int Locatevex(int vexnum, int point) {
  for (int i = 0; i < vexnum; i++) {
    if (assists[i].value == point) {
      return i;
    }
  }
  return -1;
}

int main() {
  MGraph G;
  CreateUDN(&G);
  miniSpanTreePrim(G, 1);
  // 6,10
  // 1
  // 2
  // 3
  // 4
  // 5
  // 6
  // 1,2,6
  // 1,3,1
  // 1,4,5
  // 2,3,5
  // 2,5,3
  // 3,4,5
  // 3,5,6
  // 3,6,4
  // 4,6,2
  // 5,6,6
  // v1 v3
  // v3 v6
  // v6 v4
  // v3 v2
  // v2 v5

  int arcnum, vexnum;
  edge edges;
  CreateUDN1(&edges, &vexnum, &arcnum);
  // 对连通网中的所有边进行升序排序，结果仍保存在edges数组中
  qsort(edges, arcnum, sizeof(edges[0]), cmp);
  // 创建一个空的结构体数组，用于存放最小生成树
  edge minTree;
  // 设置一个用于记录最小生成树中边的数量的常量
  int num = 0;
  // 遍历所有的边
  for (int i = 0; i < arcnum; i++) {
    // 找到边的起始顶点和结束顶点在数组assists中的位置
    int initial = Locatevex(vexnum, edges[i].initial);
    int end = Locatevex(vexnum, edges[i].end);
    // 如果顶点位置存在且顶点的标记不同，说明不在一个集合中，不会产生回路
    if (initial != -1 && end != -1 &&
        assists[initial].sign != assists[end].sign) {
      // 记录该边，作为最小生成树的组成部分
      minTree[num] = edges[i];
      // 计数+1
      num++;
      // 将新加入生成树的顶点标记全不更改为一样的
      for (int k = 0; k < vexnum; k++) {
        if (assists[k].sign == assists[end].sign) {
          assists[k].sign = assists[initial].sign;
        }
      }
      // 如果选择的边的数量和顶点数相差1，证明最小生成树已经形成，退出循环
      if (num == vexnum - 1) {
        break;
      }
    }
  }
  // 输出语句
  for (int i = 0; i < vexnum - 1; i++) {
    printf("%d,%d\n", minTree[i].initial, minTree[i].end);
  }
  // 输入连通网的边数：
  // 6 10
  // 输入连通网的顶点：
  // 1
  // 2
  // 3
  // 4
  // 5
  // 6
  // 输入各边的起始点和终点及权重：
  // 1,2,6
  // 1,3,1
  // 1,4,5
  // 2,3,5
  // 2,5,3
  // 3,4,5
  // 3,5,6
  // 3,6,4
  // 4,6,2
  // 5,6,6
  // 1,3
  // 4,6
  // 2,5
  // 3,6
  // 2,3

  return 0;
}