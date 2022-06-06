#include <stdio.h>

// 在一个网（有权图）中，求一个顶点到另一个顶点的最短路径的计算方式有两种：
// 迪杰斯特拉（Dijkstra算法）和弗洛伊德（Floyd）算法。
// 迪杰斯特拉算法计算的是有向网中的某个顶点到其余所有顶点的最短路径；
// 弗洛伊德算法计算的是任意两顶点之间的最短路径。
// 最短路径算法既适用于有向网，也同样适用于无向网。
// 迪杰斯特拉算法计算的是从网中一个顶点到其它顶点之间的最短路径问题。
// 总之，迪杰斯特拉算法解决的是从网中的一个顶点到所有其它顶点之间的最短路径，
// 算法整体的时间复杂度为O(n^2)。
// 但是如果需要求任意两顶点之间的最短路径，使用迪杰斯特拉算法虽然最终虽然也能解决问题，
// 但是大材小用，相比之下使用弗洛伊德算法解决此类问题会更合适。

#define MAX_VERtEX_NUM 20  // 顶点的最大个数
#define VRType int         // 表示弧的权值的类型
#define VertexType int     // 图中顶点的数据类型
#define INFINITY 65535
typedef struct {
  VertexType vexs[MAX_VERtEX_NUM];  // 存储图中顶点数据
  VRType arcs[MAX_VERtEX_NUM][MAX_VERtEX_NUM];  // 二维数组，记录顶点之间的关系
  int vexnum, arcnum;  // 记录图的顶点数和弧（边）数
} MGraph;
typedef int PathMatrix[MAX_VERtEX_NUM];  // 用于存储最短路径中经过的顶点的下标
typedef int ShortPathTable[MAX_VERtEX_NUM];  // 用于存储各个最短路径的权值和
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
// 构造有向网
void CreateUDG(MGraph *G) {
  scanf("%d,%d", &(G->vexnum), &(G->arcnum));
  for (int i = 0; i < G->vexnum; i++) {
    scanf("%d", &(G->vexs[i]));
  }
  for (int i = 0; i < G->vexnum; i++) {
    for (int j = 0; j < G->vexnum; j++) {
      G->arcs[i][j] = INFINITY;
    }
  }
  for (int i = 0; i < G->arcnum; i++) {
    int v1, v2, w;
    scanf("%d,%d,%d", &v1, &v2, &w);
    int n = LocateVex(G, v1);
    int m = LocateVex(G, v2);
    if (m == -1 || n == -1) {
      printf("no this vertex\n");
      return;
    }
    G->arcs[n][m] = w;
  }
}
// 迪杰斯特拉算法，v0表示有向网中起始点所在数组中的下标
void ShortestPath_Dijkstra(MGraph G, int v0, PathMatrix *p, ShortPathTable *D) {
  int final[MAX_VERtEX_NUM];  // 用于存储各顶点是否已经确定最短路径的数组
  // 对各数组进行初始化
  for (int v = 0; v < G.vexnum; v++) {
    final[v] = 0;
    (*D)[v] = G.arcs[v0][v];
    (*p)[v] = 0;
  }
  // 由于以v0位下标的顶点为起始点，所以不用再判断
  (*D)[v0] = 0;
  final[v0] = 1;
  int k = 0;
  for (int i = 0; i < G.vexnum; i++) {
    int min = INFINITY;
    // 选择到各顶点权值最小的顶点，即为本次能确定最短路径的顶点
    for (int w = 0; w < G.vexnum; w++) {
      if (!final[w]) {
        if ((*D)[w] < min) {
          k = w;
          min = (*D)[w];
        }
      }
    }
    // 设置该顶点的标志位为1，避免下次重复判断
    final[k] = 1;
    // 对v0到各顶点的权值进行更新
    for (int w = 0; w < G.vexnum; w++) {
      if (!final[w] && (min + G.arcs[k][w] < (*D)[w])) {
        (*D)[w] = min + G.arcs[k][w];
        (*p)[w] = k;  // 记录各个最短路径上存在的顶点
      }
    }
  }
}

// 迪杰斯特拉算法主要解决从网（带权图）中某一顶点计算到其它顶点之间的最短路径问题。
// 如果求有向网中每一对顶点之间的最短路径，使用迪杰斯特拉算法的解决思路是：
// 以每一个顶点为源点，执行迪杰斯特拉算法。这样可以求得每一对顶点之间的最短路径。
// 弗洛伊德算法相比于使用迪杰斯特拉算法在解决此问题上的时间复杂度虽然相同，
// 都为O(n^3)，但是弗洛伊德算法的实现形式更简单。
// 弗洛伊德的核心思想是：对于网中的任意两个顶点（例如顶点A到顶点B）来说，之间的最短路径不外乎有2种情况：
// 1.直接从顶点A到顶点B的弧的权值为顶点A到顶点B的最短路径；
// 2.从顶点A开始，经过若干个顶点，最终达到顶点B，期间经过的弧的权值和为顶点A到顶点B的最短路径。
// 所以，弗洛伊德算法的核心为：对于从顶点A到顶点B的最短路径，拿出网中所有的顶点进行如下判断：
//  Dis（A，K）+ Dis（K，B）< Dis（A，B）
// 其中，K表示网中所有的顶点；Dis（A，B）表示顶点A到顶点B的距离。
// 也就是说，拿出所有的顶点K，判断经过顶点K是否存在一条可行路径比直达的路径的权值小，如果式子成立，
// 说明确实存在一条权值更小的路径，此时只需要更新记录的权值和即可。
// 任意的两个顶点全部做以上的判断，最终遍历完成后记录的最终的权值即为对应顶点之间的最短路径。

// 用于存储最短路径中经过的顶点的下标
typedef int PathMatrix1[MAX_VERtEX_NUM][MAX_VERtEX_NUM];
// 用于存储各个最短路径的权值和
typedef int ShortPathTable1[MAX_VERtEX_NUM][MAX_VERtEX_NUM];
// 弗洛伊德算法，其中P二维数组存放各对顶点的最短路径经过的顶点，D二维数组存储各个顶点之间的权值
void ShortestPath_Floyed(MGraph G, PathMatrix1 *P, ShortPathTable1 *D) {
  // 对P数组和D数组进行初始化
  for (int v = 0; v < G.vexnum; v++) {
    for (int w = 0; w < G.vexnum; w++) {
      (*D)[v][w] = G.arcs[v][w];
      (*P)[v][w] = -1;
    }
  }
  // 拿出每个顶点作为遍历条件
  for (int k = 0; k < G.vexnum; k++) {
    // 对于第k个顶点来说，遍历网中任意两个顶点，判断间接的距离是否更短
    for (int v = 0; v < G.vexnum; v++) {
      for (int w = 0; w < G.vexnum; w++) {
        // 判断经过顶点k的距离是否更短，如果判断成立，则存储距离更短的路径
        if ((*D)[v][w] > (*D)[v][k] + (*D)[k][w]) {
          (*D)[v][w] = (*D)[v][k] + (*D)[k][w];
          (*P)[v][w] = k;
        }
      }
    }
  }
}

int main() {
  MGraph G;
  CreateUDG(&G);
  PathMatrix P;
  ShortPathTable D;
  ShortestPath_Dijkstra(G, 0, &P, &D);
  for (int i = 1; i < G.vexnum; i++) {
    printf("V%d - V%d的最短路径中的顶点有(逆序)：", 0, i);
    printf(" V%d", i);
    int j = i;
    // 由于每一段最短路径上都记录着经过的顶点，
    // 所以采用嵌套的方式输出即可得到各个最短路径上的所有顶点
    while (P[j] != 0) {
      printf(" V%d", P[j]);
      j = P[j];
    }
    printf(" V0\n");
  }
  printf("源点到各顶点的最短路径长度为:\n");
  for (int i = 1; i < G.vexnum; i++) {
    printf("V%d - V%d : %d \n", G.vexs[0], G.vexs[i], D[i]);
  }

  PathMatrix1 P1;
  ShortPathTable1 D1;
  ShortestPath_Floyed(G, &P1, &D1);
  for (int i = 0; i < G.vexnum; i++) {
    for (int j = 0; j < G.vexnum; j++) {
      printf("%d ", P1[i][j]);
    }
    printf("\n");
  }
  for (int i = 0; i < G.vexnum; i++) {
    for (int j = 0; j < G.vexnum; j++) {
      printf("%d ", D1[i][j]);
    }
    printf("\n");
  }

  // 6,8
  // 0
  // 1
  // 2
  // 3
  // 4
  // 5
  // 0,5,100
  // 0,4,30
  // 0,2,10
  // 1,2,5
  // 2,3,50
  // 3,5,10
  // 4,3,20
  // 4,5,60
  // V0 - V1的最短路径中的顶点有(逆序)： V1 V0
  // V0 - V2的最短路径中的顶点有(逆序)： V2 V0
  // V0 - V3的最短路径中的顶点有(逆序)： V3 V4 V0
  // V0 - V4的最短路径中的顶点有(逆序)： V4 V0
  // V0 - V5的最短路径中的顶点有(逆序)： V5 V3 V4 V0
  // 源点到各顶点的最短路径长度为:
  // V0 - V1 : 65535
  // V0 - V2 : 10
  // V0 - V3 : 50
  // V0 - V4 : 30
  // V0 - V5 : 60
  // -1 -1 -1 4 -1 4
  // -1 -1 -1 2 -1 3
  // -1 -1 -1 -1 -1 3
  // -1 -1 -1 -1 -1 -1
  // -1 -1 -1 -1 -1 3
  // -1 -1 -1 -1 -1 -1
  // 65535 65535 10 50 30 60
  // 65535 65535 5 55 65535 65
  // 65535 65535 65535 50 65535 60
  // 65535 65535 65535 65535 65535 10
  // 65535 65535 65535 20 65535 30
  // 65535 65535 65535 65535 65535 65535

  return 0;
}