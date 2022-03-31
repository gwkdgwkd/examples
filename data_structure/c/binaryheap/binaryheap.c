#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 二叉堆（Binary Heap）没什么神秘，性质比二叉搜索树BST还简单。
// 其主要操作就两个，sink（下沉）和swim（上浮），用以维护二叉堆的性质。
// 其主要应用有两个，首先是一种排序方法「堆排序」，第二是一种很有用的数据结构「优先级队列」。
// 二叉堆其实就是一种特殊的二叉树（完全二叉树），只不过存储在数组里。
// 一般的链表二叉树操作节点的指针，在数组里，把数组索引作为指针。
// 二叉堆还分为最大堆和最小堆。
// 最大堆的性质是：每个节点都大于等于它的两个子节点；
// 类似的，最小堆的性质是：每个节点都小于等于它的子节点。

// 优先级队列这种数据结构有一个很有用的功能，你插入或者删除元素的时候，元素会自动排序，
// 这底层的原理就是二叉堆的操作。
// 数据结构的功能无非增删查该，优先级队列有两个主要API，
// 分别是insert插入一个元素和delMax删除最大元素（如果底层用最小堆，那么就是delMin）。

int *PriorityQueuePtr;
int pqSize = 0;
void MaxPQ(int cap) {
  // 索引0不用，所以多分配一个空间
  PriorityQueuePtr = (int *)malloc(sizeof(int) * (cap + 1));
  memset(PriorityQueuePtr, 0, sizeof(int) * (cap + 1));
}
int max() { return PriorityQueuePtr[1]; }
void exch(int a, int b) {
  int tmp = PriorityQueuePtr[a];
  PriorityQueuePtr[a] = PriorityQueuePtr[b];
  PriorityQueuePtr[b] = tmp;
}
int less(int a, int b) { return PriorityQueuePtr[a] < PriorityQueuePtr[b]; }
int parent(int root) { return root / 2; }
int left(int root) { return root * 2; }
int right(int root) { return root * 2 + 1; }
// 对于最大堆，会破坏堆性质的有有两种情况：
// 如果某个节点A比它的子节点（中的一个）小，那么A就不配做父节点，应该下去，
// 下面那个更大的节点上来做父节点，这就是对A进行下沉。
// 如果某个节点A比它的父节点大，那么A不应该做子节点，应该把父节点换下来，自己去做父节点，这就是对A的上浮。
// 当然，错位的节点A可能要上浮（或下沉）很多次，才能到达正确的位置，恢复堆的性质。
void swim(int k) {
  // 如果浮到堆顶，就不能在上浮了
  while (k > 1 && less(parent(k), k)) {
    // 如果第k个元素比上层大，将k换上去
    exch(parent(k), k);
    k = parent(k);
  }
}
// 下沉比上浮略微复杂一点，因为上浮某个节点A，只需要A和其父节点比较大小即可；
// 但是下沉某个节点A，需要A和其两个子节点比较大小，
// 如果A不是最大的就需要调整位置，要把较大的那个子节点和A交换。
void sink(int k) {
  // 如果沉到堆底，就沉不下去了
  while (left(k) <= pqSize) {
    // 先假设左边节点较大
    int older = left(k);
    // 如果右边节点存在，比一下大小
    if (right(k) <= pqSize && less(older, right(k))) {
      older = right(k);
    }
    // 节点k比两孩子都大，就不必下沉了
    if (less(older, k)) break;
    // 否则，不符合最大堆的结构，下沉k节点
    exch(k, older);
    k = older;
  }
}
// insert方法先把要插入的元素添加到堆底的最后，然后让其上浮到正确位置。
void insert(int e) {
  PriorityQueuePtr[++pqSize] = e;  // 先把新元素加到最后
  swim(pqSize);                    // 然后让他上浮到正确的位置
}
// delMax方法先把堆顶元素A和堆底最后的元素B对调，然后删除A，最后让B下沉到正确位置。
int delMax() {
  int max = PriorityQueuePtr[1];  // 堆顶就是最大元素
  exch(1, pqSize--);  // 把这个最大元素换到最后，删除之
  sink(1);
  return max;
}
// 插入和删除元素的时间复杂度为O(logK)，K为当前二叉堆（优先级队列）中的元素总数。
// 因为我们时间复杂度主要花费在sink或者swim上，而不管上浮还是下沉，最多也就树（堆）的高度，也就是log级别。

void printqp() {
  for (int i = 0; i <= pqSize; ++i) {
    printf("%d ", PriorityQueuePtr[i]);
  }
  printf("\n");
}

int main() {
  MaxPQ(10);

  insert(99);
  printqp();
  insert(10);
  printqp();
  insert(80);
  printqp();
  insert(85);
  printqp();
  insert(11);
  printqp();
  insert(9);
  printqp();
  insert(20);
  printqp();
  insert(21);
  printqp();
  insert(100);
  printqp();

  while (pqSize > 0) {
    printf("delMax: %d\n", delMax());
  }

  // 0 99
  // 0 99 10
  // 0 99 10 80
  // 0 99 85 80 10
  // 0 99 85 80 10 11
  // 0 99 85 80 10 11 9
  // 0 99 85 80 10 11 9 20
  // 0 99 85 80 21 11 9 20 10
  // 0 100 99 80 85 11 9 20 10 21
  // delMax: 100
  // delMax: 99
  // delMax: 85
  // delMax: 80
  // delMax: 21
  // delMax: 20
  // delMax: 11
  // delMax: 10
  // delMax: 9
}