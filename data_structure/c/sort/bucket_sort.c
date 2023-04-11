#include <stdio.h>
#include <stdlib.h>

// 桶排序又称箱排序，是基于分治思想、效率很高的排序算法，理想情况下的时间复杂度为O(n)。

// 桶排序算法的实现思路，假设一种场景，对{5,2,1,4,3}升序排序，桶排序算法的实现思路是：
// 1.准备5个桶，从1~5对它们进行编号；
// 2.将待排序序列的各个元素放置到相同编号的桶中；
// 3.从1号桶开始，依次获取桶中放置的元素，得到的就是一个升序序列。

// 桶排序中待排序的数据量和桶的数量并不一定是一对一的关系，更多场景中是多对一的关系。
// 当存在一个桶中有多个元素的情况时，要先使用合适的排序算法对各个痛内的元素进行排序，
// 然后再根据桶的次序逐一取出所有元素，最终得到的才是一个有序序列。
// 总之，桶排序算法的实现思路是：
// 将待排序序列中的元素根据规则分组，每一组采用快排、插入排序等算法进行排序，
// 然后再按照次序将所有元素合并，就可以得到一个有序序列。

// 桶排序的时间复杂度为O(n)：
// 如果要排序的数据有n个，把它们均匀地划分到m个桶内，每个桶里就有k=n/m个元素。
// 每个桶内部使用归并排序，时间复杂度为O(k*logk)，m个桶排序的时间复杂度就是O(m*k*logk)，
// 因为k=n/m，所以整个桶排序的时间复杂度就是O(n*log(n/m))。
// 当桶的个数m接近数据个数n时，log(n/m)就是一个非常小的常量，这时的时间复杂度接近O(n)。
// 所以，桶排序的时间复杂度，取决与对各个桶之间数据进行排序的时间复杂度，桶划分的越小，
// 各个桶之间的数据越少，排序所用的时间也会越少，但相应的空间消耗就会增大。

// 桶排序对要排序的数据的要求是非常苛刻的，使用条件如下：
// 1.首先，要排序的数据需要很容易就能划分成m个桶，并且，桶与桶之间有着天然的大小顺序，
//   这样每个桶内数据都排序完之后，桶与桶之间的数据不需要在进行排序。
// 2.其次，数据在各个桶之间的分布比较均匀的，如果数据经过桶的划分之后，有些桶里的数据非常多，
//   有些非常少，很不平均，那桶内数据排序的时间复杂度就不是常量级了，
//   在极端情况下，如果数据都被划分到一个桶里，那就退化为O(nlogn)的排序算法了。
// 所以，桶排序比较适合用在外部排序中，所谓的外部排序就是数据存储在外部磁盘中，
// 数据量比较大，内存有限，无法将数据全部加载到内存中。

#define NBUCKET 6    // 桶的数量
#define INTERVAL 10  // 每个桶能存放的元素个数
struct Node {        // 建立桶
  float data;
  struct Node *next;
};
// 自定义的排序算法，用于对各个桶内元素进行排序
struct Node *InsertionSort(struct Node *list) {
  struct Node *k, *nodeList;
  if (list == NULL || list->next == NULL) {
    return list;
  }
  nodeList = list;
  k = list->next;
  nodeList->next = NULL;
  while (k != NULL) {
    struct Node *ptr;
    if (nodeList->data > k->data) {
      struct Node *tmp;
      tmp = k;
      k = k->next;
      tmp->next = nodeList;
      nodeList = tmp;
      continue;
    }
    for (ptr = nodeList; ptr->next != 0; ptr = ptr->next) {
      if (ptr->next->data > k->data) break;
    }
    if (ptr->next != 0) {
      struct Node *tmp;
      tmp = k;
      k = k->next;
      tmp->next = ptr->next;
      ptr->next = tmp;
      continue;
    } else {
      ptr->next = k;
      k = k->next;
      ptr->next->next = 0;
      continue;
    }
  }
  return nodeList;
}
// 桶排序，arr为待排序序列
void BucketSort(float *arr, int size) {
  int i, j;
  struct Node **buckets;
  // 创建所有桶
  buckets = (struct Node **)malloc(sizeof(struct Node *) * NBUCKET);
  for (i = 0; i < NBUCKET; ++i) {  // 设置每个桶为空桶
    buckets[i] = NULL;
  }

  for (i = 0; i < size; ++i) {  // 根据规定，将arr中的每个元素分散存储到各个桶中
    struct Node *current;
    int pos = arr[i] * 10;  // 根据规则，确定元素所在的桶
    // 创建存储该元素的存储块，并连接到指定的桶中
    current = (struct Node *)malloc(sizeof(struct Node));
    current->data = arr[i];
    current->next = buckets[pos];
    buckets[pos] = current;
  }

  for (i = 0; i < NBUCKET; ++i) {  // 调用自定义的排序算法，对各个桶进行排序
    buckets[i] = InsertionSort(buckets[i]);
  }

  for (j = 0, i = 0; i < NBUCKET; ++i) {  // 合并所有桶内的元素
    struct Node *node;
    node = buckets[i];
    while (node) {
      arr[j++] = node->data;
      node = node->next;
    }
  }
}
void print(float *ar, int size) {
  int i;
  for (i = 0; i < size; ++i) {
    printf("%.2f ", ar[i]);
  }
  printf("\n");
}

int main() {
  float array[] = {0.42, 0.32, 0.23, 0.52, 0.25, 0.47, 0.51};
  BucketSort(array, sizeof(array) / sizeof(float));
  print(array, sizeof(array) / sizeof(float));
  // 0.23 0.25 0.32 0.42 0.47 0.51 0.52

  return 0;
}
