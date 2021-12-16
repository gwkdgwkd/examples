// 在连通网中查找最小生成树的常用方法有两个，分别称为普里姆算法和克鲁斯卡尔算法。
// 克鲁斯卡尔算法查找最小生成树的方法是：
// 将连通网中所有的边按照权值大小做升序排序，从权值最小的边开始选择，只要此边不和已选择的边一起构成环路，就可以选择它组成最小生成树。
// 对于N个顶点的连通网，挑选出N-1条符合条件的边，这些边组成的生成树就是最小生成树。

// 实现克鲁斯卡尔算法的难点在于“如何判断一个新边是否会和已选择的边构成环路”，这里教大家一种判断的方法：
// 初始状态下，为连通网中的各个顶点配置不同的标记。
// 对于一个新边，如果它两端顶点的标记不同，就不会构成环路，可以组成最小生成树。
// 一旦新边被选择，需要将它的两个顶点以及和它直接相连的所有已选边两端的顶点改为相同的标记；
// 反之，如果新边两端顶点的标记相同，就表示会构成环路。

#include <stdio.h>
#include <stdlib.h>
#define N 9  // 图中边的数量
#define P 6  // 图中顶点的数量
// 构建表示边的结构体
struct edge {
  // 一条边有2个顶点
  int initial;
  int end;
  // 边的权值
  int weight;
};
// qsort排序函数中使用，使edges结构体中的边按照权值大小升序排序
int cmp(const void* a, const void* b) {
  return ((struct edge*)a)->weight - ((struct edge*)b)->weight;
}
// 克鲁斯卡尔算法寻找最小生成树，edges存储用户输入的图的各个边，minTree用于记录组成最小生成树的各个边
void kruskal_MinTree(struct edge edges[], struct edge minTree[]) {
  int i, initial, end, elem, k;
  // 每个顶点配置一个标记值
  int assists[P];
  int num = 0;
  // 初始状态下，每个顶点的标记都不相同
  for (i = 0; i < P; i++) {
    assists[i] = i;
  }
  // 根据权值，对所有边进行升序排序
  qsort(edges, N, sizeof(edges[0]), cmp);
  // 遍历所有的边
  for (i = 0; i < N; i++) {
    // 找到当前边的两个顶点在assists数组中的位置下标
    initial = edges[i].initial - 1;
    end = edges[i].end - 1;
    // 如果顶点位置存在且顶点的标记不同，说明不在一个集合中，不会产生回路
    if (assists[initial] != assists[end]) {
      // 记录该边，作为最小生成树的组成部分
      minTree[num] = edges[i];
      // 计数+1
      num++;
      elem = assists[end];
      // 将新加入生成树的顶点标记全部改为一样的
      for (k = 0; k < P; k++) {
        if (assists[k] == elem) {
          assists[k] = assists[initial];
        }
      }
      // 如果选择的边的数量和顶点数相差1，证明最小生成树已经形成，退出循环
      if (num == P - 1) {
        break;
      }
    }
  }
}
void display(struct edge minTree[]) {
  int cost = 0, i;
  printf("最小生成树为:\n");
  for (i = 0; i < P - 1; i++) {
    printf("%d-%d，权值：%d\n", minTree[i].initial, minTree[i].end,
           minTree[i].weight);
    cost += minTree[i].weight;
  }
  printf("总权值为：%d\n", cost);
}
int main() {
  int i;
  struct edge edges[N], minTree[P - 1];
  for (i = 0; i < N; i++) {
    scanf("%d %d %d", &edges[i].initial, &edges[i].end, &edges[i].weight);
  }
  kruskal_MinTree(edges, minTree);
  display(minTree);

  // 5 1 7
  // 5 3 8
  // 1 2 6
  // 1 3 3
  // 3 2 4
  // 3 4 3
  // 2 4 2
  // 2 6 5
  // 4 6 2
  // 最小生成树为:
  // 2-4  权值：2
  // 4-6  权值：2
  // 1-3  权值：3
  // 3-4  权值：3
  // 5-1  权值：7
  // 总权值为：17

  return 0;
}