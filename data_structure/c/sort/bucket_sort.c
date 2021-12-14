#include <stdio.h>
#include <stdlib.h>

// 桶排序（又称箱排序）是一种基于分治思想、效率很高的排序算法，理想情况下对应的时间复杂度为O(n)。

// 桶排序算法的实现思路，假设一种场景，对{5, 2, 1, 4, 3}进行升序排序，桶排序算法的实现思路是：
//  准备5个桶，从1~5对它们进行编号；
//  将待排序序列的各个元素放置到相同编号的桶中；
//  从1号桶开始，依次获取桶中放置的元素，得到的就是一个升序序列。

// 桶排序算法中，待排序的数据量和桶的数量并不一定是简单的“一对一”的关系，更多场景中是“多对一”的关系。
// 当存在“一个桶中有多个元素”的情况时，要先使用合适的排序算法对各个痛内的元素进行排序，
// 然后再根据桶的次序逐一取出所有元素，最终得到的才是一个有序序列。
// 总之，桶排序算法的实现思路是：将待排序序列中的元素根据规则分组，每一组采用快排、插入排序等算法进行排序，
// 然后再按照次序将所有元素合并，就可以得到一个有序序列。

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
