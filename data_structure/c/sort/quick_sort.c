#include <stdio.h>
#include <stdlib.h>

// C语言中自带函数库中就有快速排序——qsort函数 ，包含在<stdlib.h>头文件中。
// 快速排序算法是在起泡排序的基础上进行改进的一种算法，其实现的基本思想是：
//  通过一次排序将整个无序表分成相互独立的两部分，其中一部分中的数据都比另一部分中包含的数据的值小，然后继续沿用此方法分别对两部分进行同样的操作，
//  直到每一个小部分不可再分，所得到的整个序列就成为了有序序列。

// 例如，对无序表{49，38，65，97，76，13，27，49}进行快速排序，大致过程为：
//  首先从表中选取一个记录的关键字作为分割点（称为“枢轴”或者支点，一般选择第一个关键字），例如选取49；
//  将表格中大于49个放置于49的右侧，小于49的放置于49的左侧，假设完成后的无序表为：{27，38，13，49，65，97，76，49}；
//  以49为支点，将整个无序表分割成了两个部分，分别为{27，38，13}和{65，97，76，49}，继续采用此种方法分别对两个子表进行排序；
//  前部分子表以27为支点，排序后的子表为{13，27，38}，此部分已经有序；后部分子表以 65 为支点，排序后的子表为{49，65，97，76}；
//  此时前半部分子表中的数据已完成排序；后部分子表继续以 65为支点，将其分割为{49}和{97，76}，前者不需排序，后者排序后的结果为{76，97}；
//  通过以上几步的排序，最后由子表{13，27，38}、{49}、{49}、{65}、{76，97}构成有序表：{13，27，38，49，49，65，76，97}；

// 快速排序算法的时间复杂度为O(nlogn)，是所有时间复杂度相同的排序方法中性能最好的排序算法。

#define MAX 9
// 单个记录的结构体
typedef struct { int key; } SqNote;
// 记录表的结构体
typedef struct {
  SqNote r[MAX];
  int length;
} SqList;
// 此方法中，存储记录的数组中，下标为0的位置时空着的，不放任何记录，记录从下标为1处开始依次存放
int Partition(SqList *L, int low, int high) {
  L->r[0] = L->r[low];
  int pivotkey = L->r[low].key;
  // 直到两指针相遇，程序结束
  while (low < high) {
    // high指针左移，直至遇到比pivotkey值小的记录，指针停止移动
    while (low < high && L->r[high].key >= pivotkey) {
      high--;
    }
    // 直接将high指向的小于支点的记录移动到low指针的位置。
    L->r[low] = L->r[high];
    // low指针右移，直至遇到比pivotkey值大的记录，指针停止移动
    while (low < high && L->r[low].key <= pivotkey) {
      low++;
    }
    // 直接将low指向的大于支点的记录移动到high指针的位置
    L->r[high] = L->r[low];
  }
  // 将支点添加到准确的位置
  L->r[low] = L->r[0];
  return low;
}
void QSort(SqList *L, int low, int high) {
  if (low < high) {
    // 找到支点的位置
    int pivotloc = Partition(L, low, high);
    // 对支点左侧的子表进行排序
    QSort(L, low, pivotloc - 1);
    // 对支点右侧的子表进行排序
    QSort(L, pivotloc + 1, high);
  }
}
void QuickSort(SqList *L) { QSort(L, 1, L->length); }

int main() {
  SqList *L = (SqList *)malloc(sizeof(SqList));
  L->length = 8;
  L->r[1].key = 49;
  L->r[2].key = 38;
  L->r[3].key = 65;
  L->r[4].key = 97;
  L->r[5].key = 76;
  L->r[6].key = 13;
  L->r[7].key = 27;
  L->r[8].key = 49;
  QuickSort(L);
  for (int i = 1; i <= L->length; i++) {
    printf("%d ", L->r[i].key);
  }
  printf("\n");  // 13 27 38 49 49 65 76 97

  return 0;
}