#include <stdio.h>
#include <stdlib.h>

// 静态查找表以及动态查找表中的一些查找方法，其查找的过程都无法避免同查找表中的数据进行比较，查找算法的效率很大程度取决于同表中数据的查找次数。
// 哈希表可以通过关键字直接找到数据的存储位置，不需要进行任何的比较，其查找的效率相较于前面所介绍的查找算法是更高的。
// 哈希地址只是表示在查找表中的存储位置，而不是实际的物理存储位置。f（）是一个函数，通过这个函数可以快速求出该关键字对应的的数据的哈希地址，称之为“哈希函数”。
// 在构建哈希表时，最重要的是哈希函数的设计。对于哈希表而言，冲突只能尽可能地少，无法完全避免。

// 常用的哈希函数的构造方法有6种：直接定址法、数字分析法、平方取中法、折叠法、除留余数法和随机数法。
// 直接定址法：H（key）= key或者H（key）=a * key + b    其中H（key）表示关键字为key对应的哈希地址，a和b都为常数。
// 数字分析法:如果关键字由多位字符或者数字组成，就可以考虑抽取其中的2位或者多位作为该关键字对应的哈希地址，在取法上尽量选择变化较多的位，避免冲突发生。
// 平方取中法是对关键字做平方操作，取中间得几位作为哈希地址。此方法也是比较常用的构造哈希函数的方法。
// 折叠法是将关键字分割成位数相同的几部分（最后一部分的位数可以不同），然后取这几部分的叠加和（舍去进位）作为哈希地址。此方法适合关键字位数较多的情况。
//  移位折叠是将分割后的每一小部分，按照其最低位进行对齐，然后相加。
//  间界折叠是从一端向另一端沿分割线来回折叠。
// 除留余数法：若已知整个哈希表的最大长度m，可以取一个不大于m的数p，然后对该关键字key做取余运算，即：H（key）= key % p。
//  在此方法中，对于p的取值非常重要，由经验得知p可以为不大于m的质数或者不包含小于20的质因数的合数。
// 随机数法：是取关键字的一个随机函数值作为它的哈希地址，即：H（key）=random（key），此方法适用于关键字长度不等的情况。
//  注意：这里的随机函数其实是伪随机函数，随机函数是即使每次给定的key相同，但是H（key）都是不同；而伪随机函数正好相反，每个key都对应的是固定的H（key）。

// 如此多的构建哈希函数的方法，在选择的时候，需要根据实际的查找表的情况采取适当的方法。通常考虑的因素有以下几方面：
//  关键字的长度。如果长度不等，就选用随机数法。如果关键字位数较多，就选用折叠法或者数字分析法；反之如果位数较短，可以考虑平方取中法；
//  哈希表的大小。如果大小已知，可以选用除留余数法；
//  关键字的分布情况；
//  查找表的查找频率；
//  计算哈希函数所需的时间（包括硬件指令的因素）

// 通常用的处理冲突的方法有以下几种：
// 1 开放定址法H（key）=（H（key）+ d）MOD m（其中m为哈希表的表长，d为一个增量）当得出的哈希地址产生冲突时，选取以下3种方法中的一种获取d的值，
//   然后继续计算，直到计算出的哈希地址不在冲突为止，这3种方法为：
//    线性探测法：d=1，2，3，…，m-1
//    二次探测法：d=12，-12，22，-22，32，…
//    伪随机数探测法：d=伪随机数
//   在线性探测法中，当遇到冲突时，从发生冲突位置起，每次+1，向右探测，直到有空闲的位置为止；二次探测法中，从发生冲突的位置起，按照+12，-12，+22，…如此探测，
//   直到有空闲的位置；伪随机探测，每次加上一个随机数，直到探测到空闲位置结束。
// 2 再哈希法:当通过哈希函数求得的哈希地址同其他关键字产生冲突时，使用另一个哈希函数计算，直到冲突不再发生。
// 3 链地址法:将所有产生冲突的关键字所对应的数据全部存储在同一个线性链表中。
// 4 建立一个公共溢出区:建立两张表，一张为基本表，另一张为溢出表。基本表存储没有发生冲突的数据，当关键字由哈希函数生成的哈希地址产生冲突时，就将数据填入溢出表。

// 在哈希表中进行查找的操作同哈希表的构建过程类似，其具体实现思路为：对于给定的关键字K，将其带入哈希函数中，求得与该关键字对应的数据的哈希地址，
// 如果该地址中没有数据，则证明该查找表中没有存储该数据，查找失败：如果哈希地址中有数据，就需要做进一步的证明（排除冲突的影响），找到该数据对应
// 的关键字同K进行比对，如果相等，则查找成功；反之，如果不相等，说明在构造哈希表时发生了冲突，需要根据构造表时设定的处理冲突的方法找到下一个地址，
// 同地址中的数据进行比对，直至遇到地址中数据为NULL（说明查找失败），或者比对成功。

#define HASHSIZE 7  // 定义散列表长为数组的长度
#define NULLKEY -1
typedef struct {
  int *elem;  // 数据元素存储地址，动态分配数组
  int count;  // 当前数据元素个数
} HashTable;
// 对哈希表进行初始化
void Init(HashTable *hashTable) {
  int i;
  hashTable->elem = (int *)malloc(HASHSIZE * sizeof(int));
  hashTable->count = HASHSIZE;
  for (i = 0; i < HASHSIZE; i++) {
    hashTable->elem[i] = NULLKEY;
  }
}
// 哈希函数(除留余数法)
int Hash(int data) { return data % HASHSIZE; }
// 哈希表的插入函数，可用于构造哈希表
void Insert(HashTable *hashTable, int data) {
  int hashAddress = Hash(data);  // 求哈希地址
  // 发生冲突
  while (hashTable->elem[hashAddress] != NULLKEY) {
    // 利用开放定址法解决冲突
    hashAddress = (++hashAddress) % HASHSIZE;
  }
  hashTable->elem[hashAddress] = data;
}
// 哈希表的查找算法
int Search(HashTable *hashTable, int data) {
  int hashAddress = Hash(data);                   // 求哈希地址
  while (hashTable->elem[hashAddress] != data) {  // 发生冲突
    // 利用开放定址法解决冲突
    hashAddress = (++hashAddress) % HASHSIZE;
    // 如果查找到的地址中数据为NULL，或者经过一圈的遍历回到原位置，则查找失败
    if (hashTable->elem[hashAddress] == NULLKEY || hashAddress == Hash(data)) {
      return -1;
    }
  }
  return hashAddress;
}

// 在构造哈希表的过程中，由于冲突的产生，使得哈希表的查找算法仍然会涉及到比较的过程，因此对于哈希表的查找效率仍需以平均查找长度来衡量。
// 在哈希表的查找过程中需和给定值进行比较的关键字的个数取决于以下3个因素：
//  哈希函数：哈希函数的“好坏”取决于影响出现冲突的频繁程度。但是一般情况下，哈希函数相比于后两种的影响，可以忽略不计。
//  处理冲突的方式：对于同一组关键字，设定相同的哈希函数，使用不同的处理冲突的方式得到的哈希表是不同的，表的平均查找长度也不同。
//  哈希表的装填因子：在一般情况下，当处理冲突的方式相同的情况下，其平均查找长度取决于哈希表的装满程度：装的越满，插入数据时越有可能发生冲突；反之则越小。
// 装填因子=哈希表中数据的个数/哈希表的长度，用字符α表示（是数学符号，而不是字符a）。装填因子越小，表示哈希表中空闲的位置就越多。
// 经过计算，在假设查找表中的所有数据的查找概率相等的情况下，对于表长为m，数据个数为n的哈希表：
//  其查找成功的平均查找长度约为：-1/α * ln⁡(1-α)
//  其查找不成功的平均查找长度约为：1/(1-α)
// 通过公式可以看到，哈希表的查找效率只同装填因子有关，而同哈希表中的数据的个数无关，所以在选用哈希表做查找操作时，选择一个合适的装填因子是非常有必要的。

int main() {
  int i, result;
  HashTable hashTable;
  int arr[HASHSIZE] = {13, 29, 27, 28, 26, 30, 38};
  // 初始化哈希表
  Init(&hashTable);
  // 利用插入函数构造哈希表
  for (i = 0; i < HASHSIZE; i++) {
    Insert(&hashTable, arr[i]);
  }
  // 调用查找算法
  result = Search(&hashTable, 29);
  if (result == -1)
    printf("查找失败\n");
  else
    printf("29在哈希表中的位置是:%d\n", result + 1);  // 29在哈希表中的位置是:2

  return 0;
}
