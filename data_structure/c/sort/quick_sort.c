#include <stdio.h>
#include <stdlib.h>

// 提到排序算法，多数人最先想到的就是快速排序算法。
// 快速排序算法是在分治算法基础上设计出来的一种排序算法，
// 和其它排序算法相比，快速排序算法具有效率高、耗费资源少、容易实现等优点。
// C语言中自带函数库中就有快速排序——qsort函数，包含在<stdlib.h>头文件中。

// 快速排序算法是在起泡排序的基础上进行改进的一种算法，其实现的基本思想是：
// 1.通过一次排序将整个无序表分成相互独立的两部分，
//   其中一部分中的数据都比另一部分中包含的数据的值小；
// 2.然后继续沿用此方法分别对两部分进行同样的操作，
//   直到每一个小部分不可再分，所得到的整个序列就成为了有序序列。

// 例如，对无序表{49，38，65，97，76，13，27，49}进行快速排序，大致过程为：
// 1.首先从表中选取一个记录的关键字作为分割点（称为枢轴或者支点，一般选择第一个关键字），例如选取49；
// 2.将表格中大于49个放置于49的右侧，小于49的放置于49的左侧，假设完成后的无序表为：
//   {27，38，13，49，65，97，76，49}；
// 3.以49为支点，将整个无序表分割成了两个部分，分别为{27，38，13}和{65，97，76，49}，
//   继续采用此种方法分别对两个子表进行排序；
// 4.前部分子表以27为支点，排序后的子表为{13，27，38}，此部分已经有序；
//   后部分子表以65为支点，排序后的子表为{49，65，97，76}；
// 5.此时前半部分子表中的数据已完成排序；
//   后部分子表继续以65为支点，将其分割为{49}和{97，76}，前者不需排序，后者排序后的结果为{76，97}；
// 6.通过以上几步的排序，最后由子表{13，27，38}、{49}、{49}、{65}、{76，97}构成有序表：
//   {13，27，38，49，49，65，76，97}；

// 快速排序算法的时间复杂度为O(nlogn)，是所有时间复杂度相同的排序方法中性能最好的排序算法。
// 快速排序的期望复杂度是O(nlogn)，但最坏情况下可能就会变成O(n^2)，
// 最坏情况就是每次将一组数据划分为两组的时候，
// 分界线都选在了边界上，使得划分了和没划分一样，最后就变成了普通的选择排序了。

// 快速排序的随机化版本
// 上面版本的快排在选取主元的时候，每次都选取最右边的元素。
// 当序列为有序时，会发现划分出来的两个子序列一个里面没有元素，而另一个则只比原来少一个元素。
// 为了避免这种情况，引入一个随机化量来破坏这种有序状态。
// 在随机化的快排里面，选取a[left..right]中的随机一个元素作为主元，
// 然后再进行划分，就可以得到一个平衡的划分。

int Partition(int *nums, int left, int right) {
  // 随机选取
  int rad = rand() % (right - left + 1);
  int pivotkey = nums[left + rad];
  nums[left + rad] = nums[left];
  nums[left] = pivotkey;

  // int pivotkey = nums[left];
  while (left < right) {
    while (left < right && nums[right] >= pivotkey) {
      --right;
    }
    nums[left] = nums[right];
    while (left < right && nums[left] <= pivotkey) {
      ++left;
    }
    nums[right] = nums[left];
  }
  nums[left] = pivotkey;
  return left;
}
void QuickSort(int *nums, int left, int right) {
  if (left < right) {
    int partition = Partition(nums, left, right);
    QuickSort(nums, left, partition - 1);
    QuickSort(nums, partition + 1, right);
  }
}
void print(int *list, int size) {
  for (int i = 0; i < size; i++) {
    printf("%d ", list[i]);
  }
  printf("\n");
}

int main() {
  int a[] = {35, 33, 42, 10, 14, 19, 27, 44};
  QuickSort(a, 0, sizeof(a) / sizeof(int) - 1);
  print(a, sizeof(a) / sizeof(int));
  // 10 14 19 27 33 35 42 44

  return 0;
}