#include <stdio.h>
#include <stdlib.h>

// 归并排序算法是在分治算法(分而后立)基础上设计出来的一种排序算法，对应的时间复杂度为O(nlogn)。
// 归并排序算法实现排序的思路是：
//   将整个待排序序列划分成多个不可再分的子序列，每个子序列中仅有1个元素；
//   所有的子序列进行两两合并，合并过程中完成排序操作，最终合并得到的新序列就是有序序列。

// 对于含有n个记录的无序表，首先默认表中每个记录各为一个有序表（只不过表的长度都为1），然后进行两两合并，使n个有序表变
// 为⌈n/2⌉个长度为2或者1的有序表（例如4个小有序表合并为2个大的有序表），通过不断地进行两两合并，直到得到一个长度为n的有序表为止。
// 这种归并排序方法称为：2-路归并排序。

// 归并过程中，每次得到的新的子表本身有序，所以最终得到的为有序表。
// 归并排序算法在具体实现时，首先需要将整个记录表进行折半分解，直到分解为一个记录作为单独的一张表为止，然后在进行两两合并。

// 该算法相比于堆排序和快速排序，其主要的优点是：当记录表中含有值相同的记录时，排序前和排序后在表中的相对位置不会改变。

#define MAX 8
typedef struct {
  int key;
} SqNode;
typedef struct {
  SqNode r[MAX];
  int length;
} SqList;
// SR中的记录分成两部分：下标从i至m有序，从m+1至n也有序，此函数的功能是合二为一至TR数组中，使整个记录表有序
void Merge(SqNode SR[], SqNode TR[], int i, int m, int n) {
  int j, k;
  // 将SR数组中的两部分记录按照从小到大的顺序添加至TR数组中
  for (j = m + 1, k = i; i <= m && j <= n; k++) {
    if (SR[i].key < SR[j].key) {
      TR[k] = SR[i++];
    } else {
      TR[k] = SR[j++];
    }
  }
  // 将剩余的比目前TR数组中都大的记录复制到TR数组的最后位置
  while (i <= m) {
    TR[k++] = SR[i++];
  }
  while (j <= n) {
    TR[k++] = SR[j++];
  }
}
void MSort(SqNode SR[], SqNode TR1[], int s, int t) {
  SqNode TR2[MAX];
  // 递归的出口
  if (s == t) {
    TR1[s] = SR[s];
  } else {
    int m = (s + t) / 2;  // 每次递归将记录表中记录平分，直至每个记录各成一张表
    MSort(SR, TR2, s, m);  // 将分开的前半部分表中的记录进行排序
    MSort(SR, TR2, m + 1, t);  // 将后半部分表中的记录进行归并排序
    Merge(TR2, TR1, s, m, t);  // 最后将前半部分和后半部分中的记录统一进行排序
  }
}
// 归并排序
void MergeSort(SqList *L) { MSort(L->r, L->r, 1, L->length); }

// 实现归并操作的函数，归并的2个区域分别为[p,mid]和[mid+1,q]
void merge(int *arr, int p, int mid, int q) {
  int i, j, k;
  int leftarr[100], rightarr[100];
  int numL = mid - p + 1;
  int numR = q - mid;
  // 将arr数组中[p,mid]区域内的元素逐一拷贝到leftarr数组中
  for (i = 0; i < numL; i++) {
    leftarr[i] = arr[p - 1 + i];
  }
  // 将arr数组中[mid+1,q]区域内的元素逐一拷贝到rightarr数组中
  leftarr[i] = 2147483647;
  for (i = 0; i < numR; i++) {
    rightarr[i] = arr[mid + i];
  }
  rightarr[i] = 2147483647;
  i = 0;
  j = 0;
  // 逐一比较leftarr和rightarr中的元素，每次将较小的元素拷贝到arr数组中的[p,q]区域内
  for (k = p; k <= q; k++) {
    if (leftarr[i] <= rightarr[j]) {
      arr[k - 1] = leftarr[i];
      i++;
    } else {
      arr[k - 1] = rightarr[j];
      j++;
    }
  }
}
// 实现分割操作的函数，[p,q]用于指定归并排序的区域范围，
void merge_sort(int *arr, int p, int q) {
  int mid;
  if (arr == NULL || p > q || p == q) {
    return;
  }
  mid = (p + q) / 2;
  // 将[p,q]分为[p,mid]和[mid+1,q]区域
  merge_sort(arr, p, mid);
  merge_sort(arr, mid + 1, q);
  // 对分好的[p,mid]和[mid,q]进行归并操作
  merge(arr, p, mid, q);
}
void print(int *list, int size) {
  for (int i = 0; i < size; i++) {
    printf("%d ", list[i]);
  }
  printf("\n");
}

int main() {
  int a[] = {35, 33, 42, 10, 14, 19, 27, 44};
  merge_sort(a, 1, sizeof(a) / sizeof(int));
  print(a, sizeof(a) / sizeof(int));  // 10 14 19 27 33 35 42 44

  SqList *L = (SqList *)malloc(sizeof(SqList));
  L->length = 7;
  L->r[1].key = 49;
  L->r[2].key = 38;
  L->r[3].key = 65;
  L->r[4].key = 97;
  L->r[5].key = 76;
  L->r[6].key = 13;
  L->r[7].key = 27;
  MergeSort(L);
  for (int i = 1; i <= L->length; i++) {
    printf("%d ", L->r[i].key);
  }
  printf("\n");  // 13 27 38 49 65 76 97

  return 0;
}