// 哈希查找算法又称散列查找算法，是一种借助哈希表（散列表）查找目标元素的方法，查找效率最高时对应的时间复杂度为O(1)。
// 哈希查找算法适用于大多数场景，既支持在有序序列中查找目标元素，也支持在无序序列中查找目标元素。
// 哈希表（Hash table）又称散列表，是一种存储结构，通常用来存储多个元素。
// 和其它存储结构（线性表、树等）相比，哈希表查找目标元素的效率非常高。
// 每个存储到哈希表中的元素，都配有一个唯一的标识（又称“索引”或者“键”），
// 用户想查找哪个元素，凭借该元素对应的标识就可以直接找到它，无需遍历整个哈希表。
// 多数场景中，哈希表是在数组的基础上构建的，使用数组构建哈希表，最大的好处在于：
// 可以直接将数组下标当作已存储元素的索引，不再需要为每个元素手动配置索引，极大得简化了构建哈希表的难度。

#include <stdio.h>
#define N 10  // 指定哈希表的长度
// 自定义哈希函数
int hash(int value) { return value % 10; }
// 创建哈希表
void creatHash(int arr[5], int hashArr[N]) {
  int i, index;
  // 将序列中每个元素存储到哈希表
  for (i = 0; i < 5; i++) {
    index = hash(arr[i]);
    while (hashArr[index % N] != 0) {
      index++;
    }
    hashArr[index] = arr[i];
  }
}
// 实现哈希查找算法，hashArr表示哈希表，value为要查找的目标元素
int hash_search(int* hashArr, int value) {
  int hashAdd = hash(value);  // 查找目标元素所在的索引
  while (hashArr[hashAdd] != value) {  // 如果索引位置不是目标元素，则发生了碰撞
    hashAdd = (hashAdd + 1) % N;  // 根据线性探测法，从索引位置依次向后探测
    // 如果探测位置为空，或者重新回到了探测开始的位置（即探测了一圈），则查找失败
    if (hashArr[hashAdd] == 0 || hashAdd == hash(value)) {
      return -1;
    }
  }
  // 返回目标元素所在的数组下标
  return hashAdd;
}
int main() {
  int hashAdd;
  int hashArr[N] = {0};
  int arr[5] = {};
  creatHash(arr, hashArr);
  hashAdd = hash_search(hashArr, 50);
  // 如果返回值为 -1，表明查找失败，反之则返回目标元素所在的位置
  if (hashAdd == -1) {
    printf("查找失败\n");
  } else {
    printf("查找成功，目标元素所在哈希表中的下标为：%d\n", hashAdd);
  }

  // 查找成功，目标元素所在哈希表中的下标为2

  return 0;
}
