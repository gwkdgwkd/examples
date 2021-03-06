#include <stdio.h>

// 起泡排序，别名“冒泡排序”，该算法的核心思想是将无序表中的所有记录，通过两两比较关键字，得出升序序列或者降序序列。
// 通过一趟趟的比较，一个个的“最大值”被找到并移动到相应位置，直到检测到表中数据已经有序，或者比较次数等同于表中含有记录的个数，排序结束，这就是起泡排序。
// 使用起泡排序算法，其时间复杂度同实际表中数据的无序程度有关:
//  若表中记录本身为正序存放，则整个排序过程只需进行n-1（n为表中记录的个数）次比较，且不需要移动记录；
//  若表中记录为逆序存放（最坏的情况），则需要n-1趟排序，进行n(n-1)/2次比较和数据的移动。
// 所以该算法的时间复杂度为O(n2)。

void swap(int *a, int *b) {
  int temp;
  temp = *a;
  *a = *b;
  *b = temp;
}

int main() {
  int array[8] = {49, 38, 65, 97, 76, 13, 27, 49};
  int i, j;
  int key;
  // 有多少记录，就需要多少次冒泡，当比较过程，所有记录都按照升序排列时，排序结束
  for (i = 0; i < 8; i++) {
    key = 0;  // 每次开始冒泡前，初始化key值为0
    // 每次起泡从下标为0开始，到8-i结束
    for (j = 0; j + 1 < 8 - i; j++) {
      if (array[j] > array[j + 1]) {
        key = 1;
        swap(&array[j], &array[j + 1]);
      }
    }
    // 如果key值为0，表明表中记录排序完成
    if (key == 0) {
      break;
    }
  }
  for (i = 0; i < 8; i++) {
    printf("%d ", array[i]);
  }
  printf("\n");  // 13 27 38 49 49 65 76 97

  return 0;
}
