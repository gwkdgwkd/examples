#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 基数排序不同于其他的各类排序，其他的排序方法或多或少的是通过使用比较和移动记录来实现排序，
// 而基数排序的实现不需要进行对关键字的比较，只需要对关键字进行分配与收集两种操作即可完成。
// 基数排序算法适用于对多个整数或者多个字符串进行升序或降序排序。
// 按照个位-十位-百位的顺序进行基数排序，此种方式是从最低位开始排序，所以被称为最低位优先法（简称LSD法）。
// 按照百位-十位-各位的顺序进行排序，称为最高位优先法（简称MSD法）。
// 使用该方式进行排序同最低位优先法不同的是：当无序表中的关键字进行分配后，相当于进入了多个子序列，
// 后序的排序工作分别在各个子序列中进行（最低位优先法每次分配与收集都是相对于整个序列表而言的）。
// 最高位优先法完成排序的标准为：直到每个子序列中只有一个关键字为止。

// 针对{121,432,564,23,1,45,788}，基数排序算法的具体实现过程如下：
// 1.找到序列中的最大值（如果是字符串序列，找序列中最长的字符串），记录它的位数。
//   通过观察不难判断，整个序列中的最大值为788，它由3个数字组成。
//   这就意味着，算法中需要依次根据各元素的个位数、十位数和百位数对所有元素进行排序。
// 2.找到序列中所有元素的个位数，采用计数排序算法对它们进行排序，
//   得到的新序列为{121,1,432,23,564,45,788}。
// 3.找到{121, 1, 432, 23, 564, 45, 788}中所有元素的十位数，以同样的方式进行排序，
//   得到的新序列为{1,121,23,432,45,564,788}。
// 4.找到{1, 121, 23, 432, 45, 564, 788}中所有元素的百位数，继续以同样的方法进行排序，
//   得到的新序列为{1,23,45,121,432,564,788}。
// 经过以上几个步骤，最终完成了对{121,432,564,23,1,45,788}序列的升序排序。

#define MAX 10  // 限制各个元素各数位上的值不能超过10
// 计数排序算法，place表示以指定数位为准，对序列中的元素进行排序
void countingSort(int *array, int size, int place) {
  int *output = (int *)malloc(sizeof(int) * size);
  memset(output, 0, sizeof(int) * size);
  int i;
  // 初始化一个数组,记录各个元素的出现次数
  int count[MAX] = {0};
  // 假设第一个元素指定数位上的值最大
  int max = (array[0] / place) % 10;
  // 找到真正数位上值最大的元素
  for (i = 1; i < size; i++) {
    if (((array[i] / place) % 10) > max) max = array[i];
  }
  // 统计各个元素出现的次数
  for (i = 0; i < size; i++) count[(array[i] / place) % 10]++;
  // 累加count数组中的出现次数
  for (i = 1; i < 10; i++) count[i] += count[i - 1];
  // 根据count数组中的信息，找到各个元素排序后所在位置，存储在output数组中
  for (i = size - 1; i >= 0; i--) {
    output[count[(array[i] / place) % 10] - 1] = array[i];
    count[(array[i] / place) % 10]--;
  }
  // 将output数组中的数据原封不动地拷贝到array数组中
  for (i = 0; i < size; i++) array[i] = output[i];
}
int getMax(int *array, int size) {  // 找到整个序列中的最大值
  int i, max = array[0];
  for (i = 1; i < size; i++)
    if (array[i] > max) max = array[i];
  return max;
}
void print1(int *list, int size) {
  for (int i = 0; i < size; ++i) {
    printf("%d ", list[i]);
  }
  printf("\n");
}
// 基数排序算法
void radixSort(int *array, int size) {
  // 找到序列中的最大值
  int place, max = getMax(array, size);
  // 根据最大值具有的位数，从低位依次调用计数排序算法
  for (place = 1; max / place > 0; place *= 10) {
    countingSort(array, size, place);
    print1(array, size);
  }
}

// 基数排序较宜使用链式存储结构作为存储结构，
// 相比于顺序存储结构更节省排序过程中所需要的存储空间，称之为链式基数排序。
#define MAX_NUM_OF_KEY 8  // 构成关键字的组成部分的最大个数
// 基数，例如关键字是数字，无疑由0~9组成，基数就是10；
// 如果关键字是字符串（字母组成），基数就是26
#define RADIX 10
#define MAX_SPACE 10000
// 静态链表的结点结构
typedef struct {
  int data;  // 存储的关键字
  int keys[MAX_NUM_OF_KEY];  // 存储关键字的数组（此时是一位一位的存储在数组中）
  int next;  // 做指针用，用于是静态链表，所以每个结点中存储着下一个结点所在数组中的位置下标
} SLCell;
// 静态链表结构
typedef struct {
  SLCell r[MAX_SPACE];  // 静态链表的可利用空间，其中r[0]为头结点
  // 当前所有的关键字中最大的关键字所包含的位数，
  // 例如最大关键字是百，说明所有keynum=3
  int keynum;
  int recnum;  // 静态链表的当前长度
} SLList;
typedef int ArrType[RADIX];  // 指针数组，用于记录各子序列的首尾位置
// 排序的分配算法，i表示按照分配的位次（是个位，十位还是百位），
// f表示各子序列中第一个记录和最后一个记录的位置
void Distribute(SLCell *r, int i, ArrType f, ArrType e) {
  // 初始化指针数组
  for (int j = 0; j < RADIX; j++) {
    f[j] = 0;
  }
  // 遍历各个关键字
  for (int p = r[0].next; p; p = r[p].next) {
    // 取出每个关键字的第i位，由于采用的是最低位优先法，所以，
    // 例如，第1位指的就是每个关键字的个位
    int j = r[p].keys[i];
    if (!f[j]) {
      // 如果只想该位数字的指针不存在，说明这是第一个关键字，直接记录该关键字的位置即可
      f[j] = p;
    } else {
      // 如果存在，说明之前已经有同该关键字相同位的记录，所以需要将其进行连接，
      // 将最后一个相同的关键字的next指针指向该关键字所在的位置，同时最后移动尾指针的位置。
      r[e[j]].next = p;
    }
    e[j] = p;  // 移动尾指针的位置
  }
}
// 基数排序的收集算法，即重新设置链表中各结点的尾指针
void Collect(SLCell *r, int i, ArrType f, ArrType e) {
  int j;
  // 从0开始遍历，查找头指针不为空的情况，为空表明该位没有该类型的关键字
  for (j = 0; !f[j]; j++)
    ;
  r[0].next = f[j];  // 重新设置头结点
  int t = e[j];      // 找到尾指针的位置
  while (j < RADIX) {
    for (j++; j < RADIX; j++) {
      if (f[j]) {
        r[t].next = f[j];  // 重新连接下一个位次的首个关键字
        t = e[j];          // t代表下一个位次的尾指针所在的位置
      }
    }
  }
  r[t].next = 0;  // 0表示链表结束
}
void RadixSort(SLList *L) {
  ArrType f, e;
  // 根据记录中所包含的关键字的最大位数，一位一位的进行分配与收集
  for (int i = 0; i < L->keynum; i++) {
    // 秉着先分配后收集的顺序
    Distribute(L->r, i, f, e);
    Collect(L->r, i, f, e);
  }
}
// 创建静态链表
void creatList(SLList *L) {
  int key, i = 1, j;
  scanf("%d", &key);
  while (key != -1) {
    L->r[i].data = key;
    for (j = 0; j <= L->keynum; j++) {
      L->r[i].keys[j] = key % 10;
      key /= 10;
    }
    L->r[i - 1].next = i;
    i++;
    scanf("%d", &key);
  }
  L->recnum = i - 1;
  L->r[L->recnum].next = 0;
}
// 输出静态链表
void print(SLList *L) {
  for (int p = L->r[0].next; p; p = L->r[p].next) {
    printf("%d ", L->r[p].data);
  }
  printf("\n");
}

int main(int argc, const char *argv[]) {
  int list[] = {121, 432, 564, 23, 1, 45, 788};
  radixSort(list, sizeof(list) / sizeof(int));  // 进行基数排序
  // 121 1 432 23 564 45 788
  // 1 121 23 432 45 564 788
  // 1 23 45 121 432 564 788

  SLList *L = (SLList *)malloc(sizeof(SLList));
  L->keynum = 3;
  L->recnum = 0;
  creatList(L);  // 创建静态链表
  printf("排序前：");
  print(L);

  RadixSort(L);  // 对静态链表中的记录进行基数排序

  printf("排序后：");
  print(L);

  // 50
  // 123
  // 543
  // 187
  // 49
  // 30
  // 0
  // 2
  // 11
  // 100
  // -1
  // 排序前：50 123 543 187 49 30 0 2 11 100
  // 排序后：0 2 11 30 49 50 100 123 187 543

  return 0;
}
