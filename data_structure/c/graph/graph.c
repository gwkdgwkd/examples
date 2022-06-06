#include <stdio.h>
#include <stdlib.h>

// 数据之间的关系有3种，分别是一对一、一对多和多对多，
// 前两种关系的数据可分别用线性表和树结构存储，多对多逻辑关系数据的结构用图存储。
// 与链表不同，图中存储的各个数据元素被称为顶点（而不是节点）。
// 图存储结构可细分两种表现类型，分别为无向图和有向图。

// 图存储结构基本常识
// 1.弧头和弧尾:有向图中，无箭头一端的顶点通常被称为"初始点"或"弧尾"，
//   箭头直线的顶点被称为"终端点"或"弧头"。
// 2.入度和出度:对于有向图中的一个顶点V来说，箭头指向V的弧的数量为V的入度（InDegree，记为ID(V)）；
//   箭头远离V的弧的数量为V的出度（OutDegree，记为OD(V)）。
// 3.(V1,V2)和<V1,V2>的区别:无向图中描述两顶点（V1和V2）之间的关系可以用(V1,V2)来表示，
//   而有向图中描述从V1到V2的"单向"关系用<V1,V2>来表示。
//   由于图存储结构中顶点之间的关系是用线来表示的，
//   因此(V1,V2)还可以用来表示无向图中连接V1和V2的线，又称为边；
//   同样，<V1,V2>也可用来表示有向图中从V1到V2带方向的线，又称为弧。
// 4.集合VR的含义:图中习惯用VR表示图中所有顶点之间关系的集合。
// 5.路径和回路:无论是无向图还是有向图，
//   从一个顶点到另一顶点途径的所有顶点组成的序列（包含这两个顶点），称为一条路径。
//   如果路径中第一个顶点和最后一个顶点相同，则此路径称为"回路"（或"环"）。
//   并且，若路径中各顶点都不重复，此路径又被称为"简单路径"；
//   同样，若回路中的顶点互不重复，此回路被称为"简单回路"（或简单环）。
// 6.权和网的含义:在某些实际场景中，图中的每条边（或弧）会赋予一个实数来表示一定的含义，
//   这种与边（或弧）相匹配的实数被称为"权"，而带权的图通常称为网。
// 7.子图：指的是由图中一部分顶点和边构成的图，称为原图的子图。

// 图存储结构的分类，根据不同的特征，图又可分为完全图，连通图、稀疏图和稠密图：
// 1.完全图：若图中各个顶点都与除自身外的其他顶点有关系，这样的无向图称为完全图；
//   同时，满足此条件的有向图则称为有向完全图。
//   具有n个顶点的完全图，图中边的数量为n(n-1)/2；
//   而对于具有n个顶点的有向完全图，图中弧的数量为n(n-1)。
// 2.稀疏图和稠密图：这两种图是相对存在的，即如果图中具有很少的边（或弧），
//   此图就称为"稀疏图"；反之，则称此图为"稠密图"。
// 3.稀疏和稠密的判断条件是：e<nlogn，其中e表示图中边（或弧）的数量，n表示图中顶点的数量。
//   如果式子成立，则为稀疏图；反之为稠密图。

// 从一个顶点到达另一顶点，若存在至少一条路径，则称这两个顶点是连通着的。
// 无向图中，如果任意两个顶点之间都能够连通，则称此无向图为连通图。
// 若无向图不是连通图，但图中存储某个子图符合连通图的性质，则称该子图为连通分量。
// 这里的子图指的是图中"最大"的连通子图（也称"极大连通子图"）。
// 需要注意的是，连通分量的提出是以"整个无向图不是连通图"为前提的，因为如果无向图是连通图，
// 则其无法分解出多个最大连通子图，因为图中所有的顶点之间都是连通的。
// 有向图中，若任意两个顶点Vi和Vj，满足从Vi到Vj以及从Vj到Vi都连通，
// 也就是都含有至少一条通路，则称此有向图为强连通图。
// 与此同时，若有向图本身不是强连通图，但其包含的最大连通子图具有强连通图的性质，则称该子图为强连通分量。
// 可以这样说，连通图是在无向图的基础上对图中顶点之间的连通做了更高的要求，
// 而强连通图是在有向图的基础上对图中顶点的连通做了更高的要求。

// 对连通图进行遍历，过程中所经过的边和顶点的组合可看做是一棵普通树，通常称为生成树。
// 连通图中，由于任意两顶点之间可能含有多条通路，遍历连通图的方式有多种，
// 往往一张连通图可能有多种不同的生成树与之对应。
// 连通图中的生成树必须满足以下2个条件：
// 1.包含连通图中所有的顶点；
// 2.任意两顶点之间有且仅有一条通路；
// 因此，连通图的生成树具有这样的特征，即生成树中边的数量=顶点数-1。

// 生成树是对应连通图来说，而生成森林是对应非连通图来说的。
// 非连通图可分解为多个连通分量，而每个连通分量又各自对应多个生成树（至少是1棵），
// 因此与整个非连通图相对应的，是由多棵生成树组成的生成森林。
// 因此，多个连通分量对应的多棵生成树就构成了整个非连通图的生成森林。

// 使用图结构表示的数据元素之间虽然具有多对多的关系，
// 但是同样可以采用顺序存储，也就是使用数组有效地存储图。
// 使用数组存储图时，需要使用两个数组，一个数组存放图中顶点本身的数据（一维数组），
// 另外一个数组用于存储各顶点之间的关系（二维数组）。
// 存储图中各顶点本身数据，使用一维数组就足够了；
// 存储顶点之间的关系时，要记录每个顶点和其它所有顶点之间的关系，所以需要使用二维数组。
// 不同类型的图，存储的方式略有不同，根据图有无权，可以将图划分为两大类：图和网。
// 图，包括无向图和有向图；网，是指带权的图，包括无向网和有向网。
// 存储方式的不同，指的是：
// 1.在使用二维数组存储图中顶点之间的关系时，如果顶点之间存在边或弧，
//   在相应位置用1表示，反之用0表示；
// 2.如果使用二维数组存储网中顶点之间的关系，顶点之间如果有边或者弧的存在，
//   在数组的相应位置存储其权值，反之用0表示。
#define MAX_VERtEX_NUM 20  // 顶点的最大个数
#define VRType int         // 表示顶点之间的关系的变量类型
#define InfoType char   // 存储弧或者边额外信息的指针变量类型
#define VertexType int  // 图中顶点的数据类型
typedef enum { DG, DN, UDG, UDN } GraphKind;  // 枚举图的4种类型
typedef struct {
  VRType adj;  // 对于无权图，用1或0表示是否相邻；对于带权图，直接为权值。
  InfoType *info;  // 弧或边额外含有的信息指针
} ArcCell, AdjMatrix[MAX_VERtEX_NUM][MAX_VERtEX_NUM];
typedef struct {
  VertexType vexs[MAX_VERtEX_NUM];  // 存储图中顶点数据
  AdjMatrix arcs;      // 二维数组，记录顶点之间的关系
  int vexnum, arcnum;  // 记录图的顶点数和弧（边）数
  GraphKind kind;      // 记录图的种类
} MGraph;
// 对于无向图来说，二维数组构建的二阶矩阵，实际上是对称矩阵，
// 在存储时就可以采用压缩存储的方式存储下三角或者上三角。
// 通过二阶矩阵，可以直观地判断出各个顶点的度，为该行（或该列）非0值的和。
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
// 构造有向图
void CreateDG(MGraph *G) {
  // 输入图含有的顶点数和弧的个数
  scanf("%d,%d", &(G->vexnum), &(G->arcnum));
  // 依次输入顶点本身的数据
  for (int i = 0; i < G->vexnum; i++) {
    scanf("%d", &(G->vexs[i]));
  }
  // 初始化二维矩阵，全部归0，指针指向NULL
  for (int i = 0; i < G->vexnum; i++) {
    for (int j = 0; j < G->vexnum; j++) {
      G->arcs[i][j].adj = 0;
      G->arcs[i][j].info = NULL;
    }
  }
  // 在二维数组中添加弧的数据
  for (int i = 0; i < G->arcnum; i++) {
    int v1, v2;
    // 输入弧头和弧尾
    scanf("%d,%d", &v1, &v2);
    // 确定顶点位置
    int n = LocateVex(G, v1);
    int m = LocateVex(G, v2);
    // 排除错误数据
    if (m == -1 || n == -1) {
      printf("no this vertex\n");
      return;
    }
    // 将正确的弧的数据加入二维数组
    G->arcs[n][m].adj = 1;
  }
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
// 构造有向网，和有向图不同的是二阶矩阵中存储的是权值。
void CreateUDG(MGraph *G) {
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
    int v1, v2, w;
    scanf("%d,%d,%d", &v1, &v2, &w);
    int n = LocateVex(G, v1);
    int m = LocateVex(G, v2);
    if (m == -1 || n == -1) {
      printf("no this vertex\n");
      return;
    }
    G->arcs[n][m].adj = w;
  }
}
// 构造无向网，和无向图唯一的区别就是二阶矩阵中存储的是权值
void CreateUDN(MGraph *G) {
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
    int v1, v2, w;
    scanf("%d,%d,%d", &v1, &v2, &w);
    int m = LocateVex(G, v1);
    int n = LocateVex(G, v2);
    if (m == -1 || n == -1) {
      printf("no this vertex\n");
      return;
    }
    G->arcs[n][m].adj = w;
    G->arcs[m][n].adj = w;  // 矩阵对称
  }
}
void CreateGraph(MGraph *G) {
  // 选择图的类型
  scanf("%d", &(G->kind));
  // 根据所选类型，调用不同的函数实现构造图的功能
  switch (G->kind) {
    case DG:
      return CreateDG(G);
      break;
    case DN:
      return CreateDN(G);
      break;
    case UDG:
      return CreateUDG(G);
      break;
    case UDN:
      return CreateUDN(G);
      break;
    default:
      break;
  }
}
// 输出函数
void PrintGrapth(MGraph G) {
  for (int i = 0; i < G.vexnum; i++) {
    for (int j = 0; j < G.vexnum; j++) {
      printf("%d ", G.arcs[i][j].adj);
    }
    printf("\n");
  }
}

// 通常，图更多的是采用链表存储，具体的存储方法有3种，分别是邻接表、邻接多重表和十字链表。
// 邻接表存储图的实现方式是，给图中的各个顶点独自建立一个链表，
// 用节点存储该顶点，用链表中其他节点存储各自的邻接点。
// 为了便于管理这些链表，通常会将所有链表的头节点存储到数组中（也可以用链表存储）。
// 也正因为各个链表的头节点存储的是各个顶点，因此各链表在存储邻接点数据时，
// 仅需存储该邻接顶点位于数组中的位置下标即可。
// 对于用链接表存储网（边或弧存在权）结构，还需要节点存储权的值。
// 使用邻接表计算无向图中顶点的入度和出度会非常简单，
// 只需从数组中找到该顶点然后统计此链表中节点的数量即可。
// 而使用邻接表存储有向图时，通常各个顶点的链表中存储的都是以该顶点为弧尾的邻接点，
// 因此通过统计各顶点链表中的节点数量，只能计算出该顶点的出度，而无法计算该顶点的入度。
// 对于利用邻接表求某顶点的入度，有两种方式：
// 1.遍历整个邻接表中的节点，统计数据域与该顶点所在数组位置下标相同的节点数量，即为该顶点的入度；
// 2.建立一个逆邻接表，该表中的各顶点链表专门用于存储以此顶点为弧头的所有顶点在数组中的位置下标。
// 对于具有n个顶点和e条边的无向图，邻接表中需要存储n个头结点和2e个表结点。
// 在图中边或者弧稀疏的时候，使用邻接表要比邻接矩阵更加节省空间。
#define MAX_VERTEX_NUM 20  // 最大顶点个数
#define VertexType int     // 顶点数据的类型
#define InfoType1 int      // 图中弧或者边包含的信息的类型
typedef struct ArcNode {
  int adjvex;               // 邻接点在数组中的位置下标
  struct ArcNode *nextarc;  // 指向下一个邻接点的指针
  InfoType1 *info;          // 信息域
} ArcNode;
typedef struct VNode {
  VertexType data;                 // 顶点的数据域
  ArcNode *firstarc;               // 指向邻接点的指针
} VNode, AdjList[MAX_VERTEX_NUM];  // 存储各链表头结点的数组
typedef struct {
  AdjList vertices;    // 图中顶点的数组
  int vexnum, arcnum;  // 记录图中顶点数和边或弧数
  int kind;            // 记录图的种类
} ALGraph;

// 与邻接表不同，十字链表法仅适用于存储有向图和有向网。
// 不仅如此，十字链表法还改善了邻接表计算图中顶点入度的问题。
// 十字链表存储有向图（网）的方式与邻接表有一些相同，都以图（网）中各顶点为首元节点建立多条链表，
// 同时为了便于管理，还将所有链表的首元节点存储到同一数组（或链表）中。
// 首元节点中有一个数据域和两个指针域（分别用firstin和firstout表示）：
// 1.firstin指针用于连接以当前顶点为弧头的其他顶点构成的链表；
// 2.firstout指针用于连接以当前顶点为弧尾的其他顶点构成的链表；
// 3.data用于存储该顶点中的数据；
// 由此可以看出，十字链表实质上就是为每个顶点建立两个链表，
// 分别存储以该顶点为弧头的所有顶点和以该顶点为弧尾的所有顶点。
// 存储图的十字链表中，各链表中首元节点与其他节点的结构并不相同:
// 十字链表中普通节点的存储分为5部分内容，它们各自的作用是：
// 1.tailvex用于存储以首元节点为弧尾的顶点位于数组中的位置下标；
// 2.headvex用于存储以首元节点为弧头的顶点位于数组中的位置下标；
// 3.hlink指针：用于链接下一个存储以首元节点为弧头的顶点的节点；
// 4.tlink指针：用于链接下一个存储以首元节点为弧尾的顶点的节点；
// 5.info指针：用于存储与该顶点相关的信息，例如量顶点之间的权值；
#define MAX_VERTEX_NUM 20
// #define InfoType int  // 图中弧包含信息的数据类型
#define VertexType int
typedef struct ArcBox {
  int tailvex, headvex;  // 弧尾、弧头对应顶点在数组中的位置下标
  struct ArcBox *hlik, *tlink;  // 分别指向弧头相同和弧尾相同的下一个弧
  InfoType *info;               // 存储弧相关信息的指针
} ArcBox;
typedef struct VexNode {
  VertexType data;             // 顶点的数据域
  ArcBox *firstin, *firstout;  // 指向以该顶点为弧头和弧尾的链表首个结点
} VexNode;
typedef struct {
  VexNode xlist[MAX_VERTEX_NUM];  // 存储顶点的一维数组
  int vexnum, arcnum;             // 记录图的顶点数和弧数
} OLGraph;
int LocateVex1(OLGraph *G, VertexType v) {
  int i = 0;
  // 遍历一维数组，找到变量v
  for (; i < G->vexnum; i++) {
    if (G->xlist[i].data == v) {
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
// 构建十字链表函数
void CreateDG1(OLGraph *G) {
  // 输入有向图的顶点数和弧数
  scanf("%d,%d", &(G->vexnum), &(G->arcnum));
  // 使用一维数组存储顶点数据，初始化指针域为NULL
  for (int i = 0; i < G->vexnum; i++) {
    scanf("%d", &(G->xlist[i].data));
    G->xlist[i].firstin = NULL;
    G->xlist[i].firstout = NULL;
  }
  // 构建十字链表
  for (int k = 0; k < G->arcnum; k++) {
    int v1, v2;
    scanf("%d,%d", &v1, &v2);
    // 确定v1、v2在数组中的位置下标
    int i = LocateVex1(G, v1);
    int j = LocateVex1(G, v2);
    // 建立弧的结点
    ArcBox *p = (ArcBox *)malloc(sizeof(ArcBox));
    p->tailvex = i;
    p->headvex = j;
    // 采用头插法插入新的p结点
    p->hlik = G->xlist[j].firstin;
    p->tlink = G->xlist[i].firstout;
    G->xlist[j].firstin = G->xlist[i].firstout = p;
  }
}

// 无向图的存储可以使用邻接表，但在实际使用时，如果想对图中某顶点进行实操（修改或删除），
// 由于邻接表中存储该顶点的节点有两个，因此需要操作两个节点。
// 为了提高在无向图中操作顶点的效率，使用邻接多重表。
// 邻接多重表仅适用于存储无向图或无向网。
// 邻接多重表存储无向图的方式，可看作是邻接表和十字链表的结合。
// 同邻接表和十字链表存储图的方法相同，都是独自为图中各顶点建立一张链表，
// 存储各顶点的节点作为各链表的首元节点，同时为了便于管理将各个首元节点存储到一个数组中。
// 邻接多重表采用与邻接表相同的首元节点结构。但各链表中其他节点的结构与十字链表中相同。
// 节点中各区域及功能如下：
// 1.mark：标志域，用于标记此节点是否被操作过，例如在对图中顶点做遍历操作时，
//   为了防止多次操作同一节点，mark域为0表示还未被遍历；mark为1表示该节点已被遍历；
// 2.ivex和jvex：数据域，分别存储图中各边两端的顶点所在数组中的位置下标；
// 3.ilink：指针域，指向下一个存储与ivex有直接关联顶点的节点；
// 4.jlink：指针域，指向下一个存储与jvex有直接关联顶点的节点；
// 5.info：指针域，用于存储与该顶点有关的其他信息，比如无向网中各边的权；
#define MAX_VERTEX_NUM 20  // 图中顶点的最大个数
// #define InfoType int       // 边含有的信息域的数据类型
#define VertexType int                        // 图顶点的数据类型
typedef enum { unvisited, visited } VisitIf;  // 边标志域
typedef struct EBox {
  VisitIf mark;                // 标志域
  int ivex, jvex;              // 边两边顶点在数组中的位置下标
  struct EBox *ilink, *jlink;  // 分别指向与ivex、jvex相关的下一个边
  InfoType *info;              // 边包含的其它的信息域的指针
} EBox;
typedef struct VexBox {
  VertexType data;  // 顶点数据域
  EBox *firstedge;  // 顶点相关的第一条边的指针域
} VexBox;
typedef struct {
  VexBox adjmulist[MAX_VERTEX_NUM];  // 存储图中顶点的数组
  int vexnum, degenum;  // 记录途中顶点个数和边个数的变量
} AMLGraph;

int main() {
  MGraph G;         // 建立一个图的变量
  CreateGraph(&G);  // 调用创建函数，传入地址参数
  PrintGrapth(G);   // 输出图的二阶矩阵
  // 2
  // 6,10
  // 1
  // 2
  // 3
  // 4
  // 5
  // 6
  // 1,2,5
  // 2,3,4
  // 3,1,8
  // 1,4,7
  // 4,3,5
  // 3,6,9
  // 6,1,3
  // 4,6,6
  // 6,5,1
  // 5,4,5
  // 0 5 0 7 0 0
  // 0 0 4 0 0 0
  // 8 0 0 0 0 9
  // 0 0 5 0 0 6
  // 0 0 0 5 0 0
  // 3 0 0 0 1 0

  return 0;
}
