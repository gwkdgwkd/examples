#include <stdio.h>
#include <stdlib.h>

// 插入排序算法是所有排序方法中最简单的一种算法。
// 插入排序算法的实现思路是：
// 初始状态下，将待排序序列中的第一个元素看作是有序的子序列，从第二个元素开始，
// 在不破坏子序列有序的前提下，将后续的每个元素插入到子序列中的适当位置。
// 插入排序包括直接插入排序算法、折半插入排序、2-路插入排序、表插入排序和希尔排序等

void print(int *a, int size, int index) {
  printf("%d:", index);
  for (int j = 0; j < size; j++) {
    printf("%d ", a[j]);
  }
  printf("\n");
}

// 直接插入排序是插入排序算法中的一种，采用的方法是：
// 在添加新的记录时，使用顺序查找的方式找到其要插入的位置，然后将新记录插入。
// 直接插入排序算法本身比较简洁，容易实现，该算法的时间复杂度为O(n^2)。
void InsertSort(int *nums, int size) {  // 直接插入排序函数
  for (int i = 1; i < size; i++) {
    // 若第i个元素大于i-1元素则直接插入；反之，需要找到适当的插入位置后在插入
    if (nums[i] < nums[i - 1]) {
      int temp = nums[i];  // 记录要插入的目标元素
      int j = i - 1;  // 记录目标元素所在的位置，从此位置向前开始遍历
      while (j >= 0 && temp < nums[j]) {
        // 采用顺序查找方式找到插入的位置，在查找的同时，
        // 将数组中的元素进行后移操作，给插入元素腾出空间
        nums[j + 1] = nums[j];
        --j;
      }
      nums[j + 1] = temp;  // 插入到正确位置
    }
    print(nums, size, i);  // 打印每次排序后的结果
  }
}

// 直接插入排序算法采用的是顺序查找的方式，而在查找表中数据本身有序的前提下，
// 可以使用折半查找来代替顺序查找，这种排序的算法就是折半插入排序算法。
// 折半插入排序算法相比较于直接插入排序算法，只是减少了关键字间的比较次数，
// 而记录的移动次数没有进行优化，所以该算法的时间复杂度仍是O(n^2)。
void BInsertSort(int a[], int size) {
  int i, j, low = 0, high = 0, mid;
  int temp = 0;
  for (i = 1; i < size; i++) {
    low = 0;
    high = i - 1;
    temp = a[i];
    // 采用折半查找法判断插入位置，最终变量low表示插入位置
    while (low <= high) {
      mid = (low + high) / 2;
      if (a[mid] > temp) {
        high = mid - 1;
      } else {  // 相等时，插入位置也是mid+1
        low = mid + 1;
      }
    }
    // 有序表中插入位置后的元素统一后移
    for (j = i; j > low; j--) {
      a[j] = a[j - 1];
    }
    a[low] = temp;  // 插入元素
    print(a, size, i);
  }
}

// 2-路插入排序算法是在折半插入排序的基础上对其进行改进，
// 减少其在排序过程中移动记录的次数从而提高效率。
// 具体实现思路为：
// 另外设置一个同存储记录的数组大小相同的数组d，将无序表中第一个记录添加进d[0]的位置上，
// 然后从无序表中第二个记录开始，同d[0]作比较，如果该值比d[0]大，则添加到其右侧；
// 反之添加到其左侧，在这里的数组d可以理解成一个环状数组。
// 2-路插入排序相比于折半插入排序，只是减少了移动记录的次数，
// 没有根本上避免，所以其时间复杂度仍为O(n^2)。
void insert(int arr[], int temp[], int n) {
  int i, first, final, k;
  first = final = 0;  // 分别记录temp数组中最大值和最小值的位置
  temp[0] = arr[0];
  for (i = 1; i < n; i++) {
    // 待插入元素比最小的元素小
    if (arr[i] < temp[first]) {
      first = (first - 1 + n) % n;
      temp[first] = arr[i];
    }
    // 待插入元素比最大元素大
    else if (arr[i] > temp[final]) {
      final = (final + 1 + n) % n;
      temp[final] = arr[i];
    } else {  // 插入元素比最小大，比最大小
      k = (final + 1 + n) % n;
      // 当插入值比当前值小时，需要移动当前值的位置
      while (temp[((k - 1) + n) % n] > arr[i]) {
        temp[(k + n) % n] = temp[(k - 1 + n) % n];
        k = (k - 1 + n) % n;
      }
      // 插入该值
      temp[(k + n) % n] = arr[i];
      // 因为最大值的位置改变，所以需要实时更新final的位置
      final = (final + 1 + n) % n;
    }
  }
  // 将排序记录复制到原来的顺序表里
  for (k = 0; k < n; k++) {
    arr[k] = temp[(first + k) % n];
  }
}

// 上面三种插入排序算法，其基本结构都采用数组的形式进行存储，
// 因而无法避免排序过程中产生的数据移动的问题。
// 如果想要从根本上解决只能改变数据的存储结构，改用链表存储。
// 表插入排序，即使用链表的存储结构对数据进行插入排序。
// 在对记录按照其关键字进行排序的过程中，不需要移动记录的存储位置，只需要更改结点间指针的指向。
// 在使用数组结构表示的链表中，设定数组下标为0的结点作为链表的表头结点，并令其关键字取最大整数。
// 则表插入排序的具体实现过程是：首先将链表中数组下标为1的结点和表头，
// 结点构成一个循环链表，然后将后序的所有结点按照其存储的关键字的大小，依次插入到循环链表中。
// 与直接插入排序相比只是避免了移动记录的过程（修改各记录结点中的指针域即可），
// 而插入过程中同其它关键字的比较次数并没有改变，所以表插入排序算法的时间复杂度仍是O(n^2)。
// 在表插入排序算法求得的有序表是用链表表示的，也就注定其只能进行顺序查找。
// 而如果想用折半查找的算法，就需要对链表进行再加工，即对链表中的记录进行重新排列，
// 具体做法为：遍历链表，将链表中第i个结点移动至数组的第i个下标位置中。
#define SIZE 100
typedef struct {
  int rc;    // 记录项
  int next;  // 指针项，由于在数组中，所以只需要记录下一个结点所在数组位置的下标即可。
} SLNode;
typedef struct {
  SLNode r[SIZE];  // 存储记录的链表
  int length;      // 记录当前链表长度
} SLinkListType;
// 重新排列函数
void Arrange(SLinkListType *SL) {
  int p = SL->r[0].next;  // 令p指向当前要排列的记录
  for (int i = 1; i < SL->length; i++) {
    // 如果条件成立，证明原来的数据已经移动，需要通过不断找next域，找到其真正的位置
    while (p < i) {
      p = SL->r[p].next;
    }

    int q = SL->r[p].next;  // 找到之后，令q指针指向其链表的下一个记录所在的位置
    if (p != i) {  // 条件成立，证明需要同下标为i的记录进行位置交换
      SLNode t;
      t = SL->r[p];
      SL->r[p] = SL->r[i];
      SL->r[i] = t;
      SL->r[i].next = p;  // 交换完成后，该变next的值，便于后期遍历
    }

    p = q;  // 最后令p指向下一条记录
  }
}

int main() {
  int a1[] = {3, 1, 7, 5, 2, 4, 9, 6};
  InsertSort(a1, sizeof(a1) / sizeof(int));
  // 1:1 3 7 5 2 4 9 6
  // 2:1 3 7 5 2 4 9 6
  // 3:1 3 5 7 2 4 9 6
  // 4:1 2 3 5 7 4 9 6
  // 5:1 2 3 4 5 7 9 6
  // 6:1 2 3 4 5 7 9 6
  // 7:1 2 3 4 5 6 7 9

  int a2[] = {3, 1, 7, 5, 2, 4, 9, 6};
  BInsertSort(a2, sizeof(a2) / sizeof(int));
  // 1:1 3 7 5 2 4 9 6
  // 2:1 3 7 5 2 4 9 6
  // 3:1 3 5 7 2 4 9 6
  // 4:1 2 3 5 7 4 9 6
  // 5:1 2 3 4 5 7 9 6
  // 6:1 2 3 4 5 7 9 6
  // 7:1 2 3 4 5 6 7 9

  int size = 8;
  int a3[] = {3, 1, 7, 5, 2, 4, 9, 6};
  int temp[size];
  insert(a3, temp, size);
  for (int i = 0; i < size; i++) {
    printf("%d ", a3[i]);
  }
  printf("\n");  // 1 2 3 4 5 6 7 9

  SLinkListType *SL = (SLinkListType *)malloc(sizeof(SLinkListType));
  SL->length = 6;
  SL->r[0].rc = 0;
  SL->r[0].next = 4;
  SL->r[1].rc = 49;
  SL->r[1].next = 3;
  SL->r[2].rc = 38;
  SL->r[2].next = 1;
  SL->r[3].rc = 76;
  SL->r[3].next = 0;
  SL->r[4].rc = 13;
  SL->r[4].next = 5;
  SL->r[5].rc = 27;
  SL->r[5].next = 2;
  Arrange(SL);
  for (int i = 1; i < 6; i++) {
    printf("%d ", SL->r[i].rc);
  }
  printf("\n");  // 13 27 38 49 76

  return 0;
}