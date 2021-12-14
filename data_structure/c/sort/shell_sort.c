#include <stdio.h>
#include <stdlib.h>

// 希尔排序算法又叫缩小增量排序算法，是一种更高效的插入排序算法。
// 和普通的插入排序算法相比，希尔排序算法减少了移动元素和比较元素大小的次数，从而提高了排序效率，时间效率上有很大的改进。

// 在使用直接插入排序算法时，如果表中的记录只有个别的是无序的，多数保持有序，这种情况下算法的效率也会比较高；
// 除此之外，如果需要排序的记录总量很少，该算法的效率同样会很高。希尔排序就是从这两点出发对算法进行改进得到的排序算法。
// 一般在记录的数量多的情况下，希尔排序的排序效率较直接插入排序高。

// 希尔排序的具体实现思路是：
// 1.将待排序序列划分成多个子序列，使用普通的插入排序算法对每个子序列进行排序；
// 2.按照不同的划分标准，重复执行第一步；
// 3.使用普通的插入排序算法对整个序列进行排序。

// 经过大量的研究表明，所选取的增量值最好是没有除1之外的公因子，同时整个增量数组中最后一个增量值必须等于1，
// 因为最后必须对整张表做一次直接插入排序算法。

#define SIZE 15
typedef struct {
  int key;  // 关键字的值
} SLNode;
typedef struct {
  SLNode r[SIZE];  // 存储记录的数组
  int length;      // 记录数组中记录的数量
} SqList;
// 希尔排序的实现函数，其中dk表示增值量
void ShellInsert(SqList *L, int dk) {
  // 从dk+1个记录起，每间隔dk个记录就调取一个进行直接插入排序算法
  for (int i = dk + 1; i <= L->length; i++) {
    // 如果新调取的关键字的值，比子表中最后一个记录的关键字小，说明需要将该值调换位置
    if (L->r[i].key < L->r[i - dk].key) {
      int j;
      // 记录表中，使用位置下标为0的空间存储需要调换位置的记录的值
      L->r[0] = L->r[i];
      // 做直接插入排序，如果下标为0的关键字比下标为j的关键字小，则向后一行下标为j的值，为新插入的记录腾出空间。
      for (j = i - dk; j > 0 && (L->r[0].key < L->r[j].key); j -= dk) {
        L->r[j + dk] = L->r[j];
      }
      // 跳出循环后，将新的记录值插入到腾出的空间中，即完成了一次直接插入排序
      L->r[j + dk] = L->r[0];
    }
  }
}
// 希尔排序，通过调用不同的增量值（记录），实现对多个子表分别进行直接插入排序
void ShellSort(SqList *L, int dlta[], int t) {
  for (int k = 0; k < t; k++) {
    ShellInsert(L, dlta[k]);
  }
}

void print(int *list, int size, int i) {
  if (i > 0) {
    printf("%d: ", i);
  }
  for (int i = 0; i < size; i++) {
    printf("%d ", list[i]);
  }
  printf("\n");
}
void ShellSort1(int *list, int size) {
  int temp, i, j;

  // 待排序序列如何进行划分，划分多少次，都会影响到希尔排序算法的执行效率。
  // 希尔排序算法没有固定的划分标准，推荐一种常用的方法：
  int interval = 1;  // 初始化间隔数为1
  while (interval < size / 3) {
    interval = interval * 3 + 1;  // 计算最大间隔
  }
  // 根据间隔数，不断划分序列，并对各子序列排序
  while (interval > 0) {
    for (i = interval; i < size; i++) {  // 对各个子序列做直接插入排序
      temp = list[i];
      j = i;
      while (j > interval - 1 && list[j - interval] >= temp) {
        list[j] = list[j - interval];
        j -= interval;
      }
      if (j != i) {
        list[j] = temp;
      }
      print(list, size, interval);
    }
    interval = (interval - 1) / 3;  // 计算新的间隔数，继续划分序列
  }
}

int main(int argc, const char *argv[]) {
  int a[] = {35, 33, 42, 10, 14, 19, 27, 44};
  ShellSort1(a, sizeof(a) / sizeof(int));
  // 4: 14 33 42 10 35 19 27 44
  // 4: 14 19 42 10 35 33 27 44
  // 4: 14 19 27 10 35 33 42 44
  // 4: 14 19 27 10 35 33 42 44
  // 1: 14 19 27 10 35 33 42 44
  // 1: 14 19 27 10 35 33 42 44
  // 1: 10 14 19 27 35 33 42 44
  // 1: 10 14 19 27 35 33 42 44
  // 1: 10 14 19 27 33 35 42 44
  // 1: 10 14 19 27 33 35 42 44
  // 1: 10 14 19 27 33 35 42 44

  // 用数组来存储增量值，例如5代表每间隔5个记录组成一个子表，1表示每间隔一个，也就是最后一次对整张表的直接插入排序
  int dlta[3] = {5, 3, 1};
  SqList *L = (SqList *)malloc(sizeof(SqList));
  L->r[1].key = 49;
  L->r[2].key = 38;
  L->r[3].key = 64;
  L->r[4].key = 97;
  L->r[5].key = 76;
  L->r[6].key = 13;
  L->r[7].key = 27;
  L->r[8].key = 49;
  L->r[9].key = 55;
  L->r[10].key = 4;
  L->length = 10;
  // 调用希尔排序算法
  ShellSort(L, dlta, 3);
  // 输出语句
  for (int i = 1; i <= 10; i++) {
    printf("%d ", L->r[i].key);
  }
  printf("\n");  // 4 13 27 38 49 49 55 64 76 97

  return 0;
}
