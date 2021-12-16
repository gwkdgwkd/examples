// 普里姆算法查找最小生成树的过程，采用了贪心算法的思想。
// 对于包含N个顶点的连通网，普里姆算法每次从连通网中找出一个权值最小的边，重复N-1次，由N-1条权值最小的边组成的生成树就是最小生成树。

// 如何找出N-1条权值最小的边呢？普里姆算法的实现思路是：
//   将连通网中的所有顶点分为两类（假设为A类和B类）。初始状态下，所有顶点位于B类；
//   选择任意一个顶点，将其从B类移动到A类；
//   从B类的所有顶点出发，找出一条连接着A类中的某个顶点且权值最小的边，将此边连接着的A类中的顶点移动到B类；
//   重复执行第3步，直至B类中的所有顶点全部移动到A类，恰好可以找到N-1条边。

#include <stdio.h>
#define V 6  // 记录图中顶点的个数
typedef enum { false, true } bool;
// 查找权值最小的、尚未被选择的顶点，key
// 数组记录了各顶点之间的权值数据，visited数组记录着各个顶点是否已经被选择的信息
int min_Key(int key[], bool visited[]) {
  // 遍历key数组使用，min记录最小的权值，min_index记录最小权值关联的顶点
  int min = 2147483647, min_index;
  // 遍历key数组
  for (int v = 0; v < V; v++) {
    // 如果当前顶点为被选择，且对应的权值小于min值
    if (visited[v] == false && key[v] < min) {
      // 更新min的值并记录该顶点的位置
      min = key[v];
      min_index = v;
    }
  }
  // 返回最小权值的顶点的位置
  return min_index;
}
//输出最小生成树
void print_MST(int parent[], int cost[V][V]) {
  int minCost = 0;
  printf("最小生成树为：\n");
  //遍历parent数组
  for (int i = 1; i < V; i++) {
    // parent数组下标值表示各个顶点，各个下标对应的值为该顶点的父节点
    printf("%d - %d wight:%d\n", parent[i] + 1, i + 1,
           cost[i][parent[i]]);  // 由于数组下标从0开始，因此输出时各自+1
    // 统计最小生成树的总权值
    minCost += cost[i][parent[i]];
  }
  printf("总权值为：%d", minCost);
}
// 根据用户提供了图的信息（存储在cost数组中），寻找最小生成树
void find_MST(int cost[V][V]) {  // key数组用于记录B类顶点到A类顶点的权值
  // parent数组用于记录最小生成树中各个顶点父节点的位置，便于最终生成最小生成树
  // visited数组用于记录各个顶点属于A类还是B类
  int parent[V], key[V];
  bool visited[V];
  // 初始化3个数组
  for (int i = 0; i < V; i++) {
    key[i] = 2147483647;  // 将key数组各个位置设置为无限大的数
    visited[i] = false;   // 所有的顶点全部属于B类
    parent[i] = -1;       // 所有顶点都没有父节点
  }
  // 选择key数组中第一个顶点，开始寻找最小生成树
  key[0] = 0;      // 该顶点对应的权值设为0
  parent[0] = -1;  // 该顶点没有父节点
  // 对于V个顶点的图，最需选择V-1条路径，即可构成最小生成树
  for (int x = 0; x < V - 1; x++) {
    // 从key数组中找到权值最小的顶点所在的位置
    int u = min_Key(key, visited);
    // 该顶点划分到A类
    visited[u] = true;
    // 由于新顶点加入A类，因此需要更新key数组中的数据
    for (int v = 0; v < V; v++) {
      // 如果类B中存在到下标为u的顶点的权值比key
      // 数组中记录的权值还小，表明新顶点的加入，使得类B到类A
      // 顶点的权值有了更好的选择
      if (cost[u][v] != 0 && visited[v] == false && cost[u][v] < key[v]) {
        // 更新parent数组记录的各个顶点父节点的信息
        parent[v] = u;
        // 更新key数组
        key[v] = cost[u][v];
      }
    }
  }
  // 根据parent记录的各个顶点父节点的信息，输出寻找到的最小生成树
  print_MST(parent, cost);
}
int main() {
  int p1, p2;
  int wight;
  int cost[V][V] = {0};
  printf("输入图（顶点到顶点的路径和权值)：\n");
  while (1) {
    scanf("%d %d", &p1, &p2);
    // 如果用户输入-1 -1，表示输入结束
    if (p1 == -1 && p2 == -1) {
      break;
    }
    scanf("%d", &wight);
    cost[p1 - 1][p2 - 1] = wight;
    cost[p2 - 1][p1 - 1] = wight;
  }
  // 根据用户输入的图的信息，寻找最小生成树
  find_MST(cost);
  return 0;
}
