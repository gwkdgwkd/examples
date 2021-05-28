#include <stdio.h>
#include <stdlib.h>

// 数据结构中的查找表，专用于实现各种查找算法。
// 查找表又可分为静态查找表和动态查找表。借助静态查找表可引申出顺序查找算法、折半查找算法、分块查找算法等；而记住动态查找表，也可以通过构建二叉排序树、平衡二叉树等实现查找操作。

// 查找表是由同一类型的数据元素构成的集合。一般对于查找表有以下几种操作：
//  在查找表中查找某个具体的数据元素；
//  在查找表中插入数据元素；
//  从查找表中删除数据元素；
// 在查找表中只做查找操作，而不改动表中数据元素，称此类查找表为静态查找表；反之，在查找表中做查找操作的同时进行插入数据或者删除数据的操作，称此类表为动态查找表。
// 关键字又细分为主关键字和次关键字。若某个关键字可以唯一地识别一个数据元素时，称这个关键字为主关键字，例如学生的学号就具有唯一性；反之，不具有唯一性，称为次关键字。

// 静态查找表既可以使用顺序表表示，也可以使用链表结构表示。虽然一个是数组、一个链表，但两者在做查找操作时，基本上大同小异。
// 静态查找表用顺序存储结构表示时，顺序查找的查找过程为：从表中的最后一个数据元素开始，逐个同记录的关键字做比较，如果匹配成功，则查找成功；
// 反之，如果直到表中第一个关键字查找完也没有成功匹配，则查找失败。

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
// 查找表查找的功能函数，其中key为关键字
int Search_seq(SSTable *st, keyType key) {
  // 将关键字作为一个数据元素存放到查找表的第一个位置，起监视哨的作用
  st->elem[0].key = key;
  int i = st->length;
  // 从查找表的最后一个数据元素依次遍历，一直遍历到数组下标为0
  while (st->elem[i].key != key) {
    i--;
  }
  // 如果 i=0，说明查找失败；反之，返回的是含有关键字key的数据元素在查找表中的位置
  return i;
}

// 查找操作的性能分析主要考虑其时间复杂度，而整个查找过程其实大部分时间花费在关键字和查找表中的数据进行比较上。
// 所以查找算法衡量好坏的依据为：查找成功时，查找的关键字和查找表中的数据元素中进行过比较的个数的平均值，称为平均查找长度（Average Search Length，用ASL表示）。
// 对于具有n个数据元素的查找表，查找成功的平均查找长度的计算公式为：ASL=1/n * [1,n](n-i+1)=(n+1)/2
// 1/n是概率，没有元素被查找的概率相同，总和为1。若表中有n个数据元素，查找第一个元素时需要比较n次；查找最后一个元素时需要比较1次，所以有比较次数=n–i+1 。
// 而对于查找算法来说，查找成功和查找失败的概率是相同的。所以，查找算法的平均查找长度应该为查找成功时的平均查找长度加上查找失败时的平均查找长度。
// 对于含有n个数据的表来说，每次查找失败，比较的次数都是n+1。所以查找算法的平均查找长度的计算公式为：3(n+1)/4

int main() {
  SSTable *st;
  Create(&st, 6);
  getchar();
  printf("请输入查找数据的关键字：\n");
  int key;
  scanf("%d", &key);
  int location = Search_seq(st, key);
  if (location == 0) {
    printf("查找失败\n");
  } else {
    printf("数据在查找表中的位置为：%d\n", location);
  }
  return 0;
}