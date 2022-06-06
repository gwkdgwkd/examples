#include <stdio.h>

// 对于外部排序算法来说，其直接影响算法效率的因素为读写外存的次数，即次数越多，算法效率越低。
// 若想提高算法的效率，即减少算法运行过程中读写外存的次数，可以增加k–路平衡归并中的k值。
// 但是经过计算得知，如果毫无限度地增加k值，虽然会减少读写外存数据的次数，
// 但会增加内部归并的时间，得不偿失。
// 为了避免在增加k值的过程中影响内部归并的效率，在进行k-路归并时可以使用败者树来实现，
// 该方法在增加k值时不会影响其内部归并的效率。
// 败者树是树形选择排序的一种变形，本身是一棵完全二叉树。
// 其双亲结点存储的是左右孩子比较之后的失败者，而胜利者则继续同其它的胜者去比较。
// 胜者树和败者树的区别就是：胜者树中的非终端结点中存储的是胜利的一方；
// 而败者树中的非终端结点存储的是失败的一方，而在比较过程中，都是拿胜者去比较。
// 使用败者树来实现增加k-路归并的规模来提高外部排序的整体效率。
// 但是对于k值得选择也并不是一味地越大越好，而是需要综合考虑选择一个合适的k值。

#define k 5
#define MAXKEY 10000
#define MINKEY -1
// 表示非终端结点，由于是完全二叉树，所以可以使用一维数组来表示
typedef int LoserTree[k];
typedef struct {
  int key;
} ExNode, External[k + 1];
External b;  // 表示败者树的叶子结点
// a0-a4为5个初始归并段
int a0[] = {10, 15, 16};
int a1[] = {9, 18, 20};
int a2[] = {20, 22, 40};
int a3[] = {6, 15, 25};
int a4[] = {12, 37, 48};
// t0-t4用于模拟从初始归并段中读入记录时使用
int t0 = 0, t1 = 0, t2 = 0, t3 = 0, t4 = 0;
// 沿从叶子结点b[s]到根结点ls[0]的路径调整败者树
void Adjust(LoserTree ls, int s) {
  int t = (s + k) / 2;
  while (t > 0) {
    // 判断每一个叶子结点同其双亲结点中记录的败者的值相比较，
    // 调整败者的值，其中s一直表示的都是胜者
    if (b[s].key > b[ls[t]].key) {
      int swap = s;
      s = ls[t];
      ls[t] = swap;
    }
    t = t / 2;
  }
  // 最终将胜者的值赋给ls[0]
  ls[0] = s;
}
// 创建败者树
void CreateLoserTree(LoserTree ls) {
  b[k].key = MINKEY;
  // 设置ls数组中败者的初始值
  for (int i = 0; i < k; i++) {
    ls[i] = k;
  }
  // 对于每一个叶子结点，调整败者树中非终端结点中记录败者的值
  for (int i = k - 1; i >= 0; i--) {
    Adjust(ls, i);
  }
}
// 模拟从外存向内存读入初始归并段中的每一小部分
void input(int i) {
  switch (i) {
    case 0:
      if (t0 < 3) {
        b[i].key = a0[t0];
        t0++;
      } else {
        b[i].key = MAXKEY;
      }
      break;
    case 1:
      if (t1 < 3) {
        b[i].key = a1[t1];
        t1++;
      } else {
        b[i].key = MAXKEY;
      }
      break;
    case 2:
      if (t2 < 3) {
        b[i].key = a2[t2];
        t2++;
      } else {
        b[i].key = MAXKEY;
      }
      break;
    case 3:
      if (t3 < 3) {
        b[i].key = a3[t3];
        t3++;
      } else {
        b[i].key = MAXKEY;
      }
      break;
    case 4:
      if (t4 < 3) {
        b[i].key = a4[t4];
        t4++;
      } else {
        b[i].key = MAXKEY;
      }
      break;
    default:
      break;
  }
}
// 败者树的建立及内部归并
void K_Merge(LoserTree ls) {
  // 模拟从外存中的5个初始归并段中向内存调取数据
  for (int i = 0; i <= k; i++) {
    input(i);
  }
  // 创建败者树
  CreateLoserTree(ls);
  // 最终的胜者存储在is[0]中，当其值为MAXKEY时，证明5个临时文件归并结束
  while (b[ls[0]].key != MAXKEY) {
    // 输出过程模拟向外存写的操作
    printf("%d ", b[ls[0]].key);
    // 继续读入后续的记录
    input(ls[0]);
    // 根据新读入的记录的关键字的值，重新调整败者树，找出最终的胜者
    Adjust(ls, ls[0]);
  }
}

int main(int argc, const char* argv[]) {
  LoserTree ls;
  K_Merge(ls);
  printf("\n");  // 6 9 10 12 15 15 16 18 20 20 22 25 37 40 48

  return 0;
}