#include <stdio.h>
#include <stdlib.h>

// 树形选择排序（又称“锦标赛排序”），是一种按照锦标赛的思想进行选择排序的方法，
// 即所有记录采取两两分组，筛选出较小（较大）的值；
// 然后从筛选出的较小（较大）值中再两两分组选出更小（更大）值，依次类推，直到最后选出一个最小（最大）值。
// 同样可以采用此方式筛选出次小（次大）值等。
// 整个排序的过程，可以用一棵具有n个叶子结点的完全二叉树表示。

#define N 8
void TreeSelectionSort(int *mData) {
  int MinValue = 0;
  int tree[N * 4];  // 树的大小
  int max, maxIndex, treeSize;
  int i, n = N, baseSize = 1;
  while (baseSize < n) baseSize *= 2;
  treeSize = baseSize * 2 - 1;
  for (i = 0; i < n; i++) {  // 将要排的数字填到树上
    tree[treeSize - i] = mData[i];
  }
  for (; i < baseSize; i++) {  // 其余的地方都填上最小值
    tree[treeSize - i] = MinValue;
  }
  // 构造一棵树，大的值向上构造
  for (i = treeSize; i > 1; i -= 2) {
    tree[i / 2] = (tree[i] > tree[i - 1] ? tree[i] : tree[i - 1]);
  }
  n -= 1;
  while (n != -1) {
    max = tree[1];        // 树顶为最大值
    mData[n--] = max;     // 从大到小倒着贴到原数组上
    maxIndex = treeSize;  // 最大值下标
    while (tree[maxIndex] != max) {
      maxIndex--;
    }  // 找最大值下标
    tree[maxIndex] = MinValue;
    while (maxIndex > 1) {
      if (maxIndex % 2 == 0) {
        tree[maxIndex / 2] =
            (tree[maxIndex] > tree[maxIndex + 1] ? tree[maxIndex]
                                                 : tree[maxIndex + 1]);
      } else {
        tree[maxIndex / 2] =
            (tree[maxIndex] > tree[maxIndex - 1] ? tree[maxIndex]
                                                 : tree[maxIndex - 1]);
      }
      maxIndex /= 2;
    }
  }
}

int main() {
  int a[N] = {49, 38, 65, 97, 76, 13, 27, 49};
  TreeSelectionSort(a);
  for (int i = 0; i < N; i++) {
    printf("%d ", a[i]);
  }
  printf("\n");  // 13 27 38 49 49 65 76 97

  return 0;
}