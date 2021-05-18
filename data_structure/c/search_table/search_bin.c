#include <stdio.h>
#include <stdlib.h>

// 折半查找，也称二分查找，在某些情况下相比于顺序查找，使用折半查找算法的效率更高。但是该算法的使用的前提是静态查找表中的数据必须是有序的。
// 在折半查找之前对查找表按照所查的关键字进行排序的意思是：若查找表中存储的数据元素含有多个关键字时，使用哪种关键字做折半查找，就需要提前以该关键字对所有数据进行排序。

#define keyType int
typedef struct {
  keyType key;  // 查找表中每个数据元素的值，如果需要，还可以添加其他属性
} ElemType;
typedef struct {
  ElemType *elem;  // 存放查找表中数据元素的数组
  int length;      // 记录查找表中数据的总数量
} SSTable;
// 创建查找表
void Create(SSTable **st, int length) {
  (*st) = (SSTable *)malloc(sizeof(SSTable));
  (*st)->length = length;
  (*st)->elem = (ElemType *)malloc((length + 1) * sizeof(ElemType));
  printf("输入表中的数据元素：\n");
  // 根据查找表中数据元素的总长度，在存储时，从数组下标为1的空间开始存储数据
  for (int i = 1; i <= length; i++) {
    scanf("%d", &((*st)->elem[i].key));
  }
}
// 折半查找算法
int Search_Bin(SSTable *ST, keyType key) {
  int low = 1;            // 初始状态low指针指向第一个关键字
  int high = ST->length;  // high指向最后一个关键字
  int mid;
  while (low <= high) {
    mid = (low + high) / 2;  // int本身为整形，所以，mid每次为取整的整数
    if (ST->elem[mid].key == key) {
      // 如果mid指向的同要查找的相等，返回mid所指向的位置
      return mid;
    } else if (ST->elem[mid].key > key) {
      // 如果mid指向的关键字较大，则更新high指针的位置
      high = mid - 1;
    } else {
      // 反之，则更新low指针的位置
      low = mid + 1;
    }
  }
  return 0;
}

// 折半查找的运行过程可以用二叉树来描述，这棵树通常称为“判定树”。
// 对于具有n个结点（查找表中含有n个关键字）的判定树，它的层次数至多为：log2n+1
// 在查找表中各个关键字被查找概率相同的情况下，折半查找的平均查找长度为：ASL = log2(n+1) – 1。
// 通过比较折半查找的平均查找长度，同顺序查找相对比，明显折半查找的效率要高。但是折半查找算法只适用于有序表，同时仅限于查找表用顺序存储结构表示。
// 当查找表使用链式存储结构表示时，折半查找算法无法有效地进行比较操作（排序和查找操作的实现都异常繁琐）。

int main(int argc, const char *argv[]) {
  SSTable *st;
  Create(&st, 11);
  getchar();
  printf("请输入查找数据的关键字：\n");
  int key;
  scanf("%d", &key);
  int location = Search_Bin(st, key);
  // 如果返回值为0，则证明查找表中未查到key值，
  if (location == 0) {
    printf("查找表中无该元素\n");
  } else {
    printf("数据在查找表中的位置为：%d\n", location);
  }

  // 输入表中的数据元素：
  // 5 13 19 21 37 56 64 75 80 88 92
  // 请输入查找数据的关键字：
  // 21
  // 数据在查找表中的位置为：4

  return 0;
}