#include <stdio.h>
#include <stdlib.h>

// 堆的含义：在含有n个元素的序列中，如果序列中的元素满足下面其中一种关系时，此序列可以称之为堆。
// 1.ki≤k2i且ki≤k2i+1（在n个记录的范围内，第i个关键字的值小于第2*i个关键字，同时也小于第2*i+1个关键字）
// 2.ki≥k2i且ki≥k2i+1（在n个记录的范围内，第i个关键字的值大于第2*i个关键字，同时也大于第2*i+1个关键字）
// 对于堆的定义也可以使用完全二叉树来解释，因为在完全二叉树中第i个结点的左孩子恰好是第2i个结点，右孩子恰好是2i+1个结点。
// 如果该序列可以被称为堆，则使用该序列构建的完全二叉树中，每个根结点的值都必须不小于（或者不大于）左右孩子结点的值。
// 堆用完全二叉树表示时，其表示方法不唯一，但是可以确定的是树的根结点要么是无序表中的最小值，要么是最大值。

// 通过将无序表转化为堆，可以直接找到表中最大值或者最小值，然后将其提取出来，令剩余的记录再重建一个堆，
// 取出次大值或者次小值，如此反复执行就可以得到一个有序序列，此过程为堆排序。
// 堆排序过程的代码实现需要解决两个问题：
// 1.如何将得到的无序序列转化为一个堆？
// 2.在输出堆顶元素之后（完全二叉树的树根结点），
//   如何调整剩余元素构建一个新的堆？(从根结点到叶子结点的整个调整的过程，被称为“筛选”)

// 堆分为大根堆和小根堆，大根堆是一棵完全二叉树，且对于任意一个子节点，其值均不大于父节点的值，
// 如此递推，就是根节点的值是最大的，即大根堆。
// 小根堆也是一棵完全二叉树,且对于任意一个子节点，其数值均不小于父节点的值，
// 这样层层递推，就是根节点的值最小，即小根堆。

// 完全二叉树通过一维数组(数组起始索引为0)来存储时有如下特性，当然堆就也有了：
// 1.父节点i的左子节点在位置(2*i+1);
// 2.父节点i的右子节点在位置(2*i+2);
// 3.子节点i的父节点在位置((i-1)/2);

// 堆排序在最坏的情况下，其时间复杂度仍为O(nlogn)。这是相对于快速排序的优点所在。
// 同时堆排序相对于树形选择排序，其只需要一个用于记录交换（rc）的辅助存储空间，比树形选择排序的运行空间更小。

void print(int *heap, int size) {
  for (int i = 0; i < size; ++i) {
    printf("%d ", heap[i]);
  }
  printf("\n");
}
void HeapAdjust(int *heap, int top, int size) {
  int temp = heap[top];  // 保存父节点的值

  for (int i = top * 2 + 1; i < size; i = 2 * i + 1) {
    // i是top的左孩子，i+1是top的右孩子
    if (i + 1 < size && heap[i] < heap[i + 1]) {
      ++i;  // 如果右孩子大于左孩子，i指向右孩子
    }
    if (temp > heap[i]) {
      break;  // 如果左右孩子中最大的，比父节点小，什么都不做
    }
    heap[top] = heap[i];  // 父节点等于左右孩子中最大的那个
    top = i;              // top标识目前需要更新的孩子

    // 此时的i位置所表示的孩子（左右孩子中最大并且比父亲节点大）需要被更新，
    // 下次循环，目前的节点i更新到的它的左孩子处，即i=2*i+1
  }
  heap[top] = temp;  // 此时top表示，父亲节点值（temp）应该存放的位置
}
void HeapSort(int *heap, int size) {
  // 根据无序数组创建大顶堆
  for (int i = size / 2 - 1; i >= 0; --i) {
    // size/2-1是从下标0开始的数组中（当成二叉树），从上到下，从左到右最后一个非叶子节点
    HeapAdjust(heap, i, size);
    print(heap, size);
  }

  // 从大顶堆到升序排列的数组
  for (int i = size - 1; i >= 0; --i) {
    // 数组最后的元素依次与heap[0]交换，每次把heap中最大的元素放到数组后面
    int temp = heap[i];
    heap[i] = heap[0];
    heap[0] = temp;

    // 堆定已经不是最大的元素来，调整堆
    HeapAdjust(heap, 0, i);
  }
}

int main() {
  int a[] = {49, 38, 65, 97, 76, 13, 98, 100};
  int len = sizeof(a) / sizeof(int);
  print(a, len);

  HeapSort(a, len);
  print(a, len);
}
