#include <stdio.h>
#include <stdlib.h>

// 数组即可以存储不可再分的数据元素（如数字5、字符'a'），也可以继续存储数组（即n维数组）。以上两种数据存储形式绝不会出现在同一个数组中。
// 有人可能会说，创建一个二维数组来存储{1,{1,2,3}}。在存储上确实可以实现，但无疑会造成存储空间的浪费。
// 对于存储 {1,{1,2,3}} 这样的数据，更适合用广义表结构来存储。广义表，又称列表，也是一种线性存储结构。同数组类似，广义表中既可以存储不可再分的元素，也可以存储广义表。
// 通常，广义表中存储的单个元素称为"原子"，而存储的广义表称为"子表"。

// 以下是广义表存储数据的一些常用形式：
//     A = ()：A 表示一个广义表，只不过表是空的。
//     B = (e)：广义表B中只有一个原子e。
//     C = (a,(b,c,d)) ：广义表C中有两个元素，原子a和子表(b,c,d)。
//     D = (A,B,C)：广义表D中存有3个子表，分别是A、B和C。这种表示方式等同于D = ((),(e),(b,c,d)) 。
//     E = (a,E)：广义表E中有两个元素，原子a和它本身。这是一个递归广义表，等同于：E = (a,(a,(a,…)))。
// 注意，A = ()和A = (())是不一样的。前者是空表，而后者是包含一个子表的广义表，只不过这个子表是空表。

// 当广义表不是空表时，称第一个数据（原子或子表）为"表头"，剩下的数据构成的新广义表为"表尾"。
// 强调一下，除非广义表为空表，否则广义表一定具有表头和表尾，且广义表的表尾一定是一个广义表。
// 例如在广义表中LS={1,{1,2,3},5}中，表头为原子1，表尾为子表{1,2,3}和原子5构成的广义表，即{{1,2,3},5}。
// 再比如，在广义表LS = {1}中，表头为原子1 ，但由于广义表中无表尾元素，因此该表的表尾是一个空表，用{}表示。

// 由于广义表中既可存储原子（不可再分的数据元素），也可以存储子表，因此很难使用顺序存储结构表示，通常情况下广义表结构采用链表实现。
// 使用顺序表实现广义表结构，不仅需要操作 n 维数组（例如 {1,{2,{3,4}}} 就需要使用三维数组存储），还会造成存储空间的浪费。
// 使用链表存储广义表，首先需要确定链表中节点的结构。由于广义表中可同时存储原子和子表两种形式的数据，因此链表节点的结构也有两种。

// 广义表的长度，指的是广义表中所包含的数据元素的个数。
// 由于广义表中可以同时存储原子和子表两种类型的数据，因此在计算广义表的长度时规定，广义表中存储的每个原子算作一个数据，同样每个子表也只算作是一个数据。
//  例如，在广义表{a,{b,c,d}}中，它包含一个原子和一个子表，因此该广义表的长度为2。
//  再比如，广义表{{a,b,c}}中只有一个子表{a,b,c}，因此它的长度为1。
// 广义表规定，空表{}的长度为0。

// 广义表的深度，可以通过观察该表中所包含括号的层数间接得到。
// 编写程序计算广义表的深度时，可以采用递归的方式：
//  依次遍历广义表C的每个节点，若当前节点为原子（tag值为0），则返回0；若为空表，则返回1；反之，则继续遍历该子表中的数据元素。
//  设置一个初始值为0的整形变量max，每次递归过程返回时，令max与返回值进行比较，并取较大值。这样，当整个广义表递归结束时，max+1就是广义表的深度。
// 其实，每次递归返回的值都是当前所在的子表的深度，原子默认深度为0，空表默认深度为1。

// 对于任意一个非空广义表来说，都是由两部分组成：表头和表尾。反之，只要确定的一个广义表的表头和表尾，那么这个广义表就可以唯一确定下来。
// 复制一个广义表，也是不断的复制表头和表尾的过程。如果表头或者表尾同样是一个广义表，依旧复制其表头和表尾。
// 所以，复制广义表的过程，其实就是不断的递归，复制广义表中表头和表尾的过程。
// 递归的出口有两个：
//  如果当前遍历的数据元素为空表，则直接返回空表。
//  如果当前遍历的数据元素为该表的一个原子，那么直接复制，返回即可。

// 无论采用下面哪一种节点结构存储广义表，都不要破坏广义表中各数据元素之间的并列关系。
// 拿{a,{b,c,d}}来说，原子a和子表{b,c,d}是并列的，而在子表{b,c,d}中原子b、c、d是并列的。
#if 1
typedef struct GLNode {
  int tag;      // 标志域，tag标记位用于区分此节点是原子还是子表，通常原子的tag值为0，子表的tag值为1。
  union {       // 用到了union共用体，因为同一时间此节点不是原子节点就是子表节点，当表示原子节点时，就使用atom变量；反之则使用ptr结构体。
    char atom;  // 原子结点的值域
    struct {
      struct GLNode *hp, *tp;
    } ptr;  // 子表结点的指针域，hp指向表头；tp指向表尾
    // 子表节点中的hp指针用于连接本子表中存储的原子或子表，tp指针用于连接广义表中下一个原子或子表。
  };
} * Glist, GNode;

Glist creatGlist(Glist C) {
  // 广义表C
  C = (Glist)malloc(sizeof(Glist));
  C->tag = 1;
  // 表头原子‘a’
  C->ptr.hp = (Glist)malloc(sizeof(Glist));
  C->ptr.hp->tag = 0;
  C->ptr.hp->atom = 'a';
  // 表尾子表（b,c,d）,是一个整体
  C->ptr.tp = (Glist)malloc(sizeof(Glist));
  C->ptr.tp->tag = 1;
  C->ptr.tp->ptr.hp = (Glist)malloc(sizeof(Glist));
  C->ptr.tp->ptr.tp = NULL;
  // 开始存放下一个数据元素（b,c,d）,表头为‘b’，表尾为（c,d）
  C->ptr.tp->ptr.hp->tag = 1;
  C->ptr.tp->ptr.hp->ptr.hp = (Glist)malloc(sizeof(Glist));
  C->ptr.tp->ptr.hp->ptr.hp->tag = 0;
  C->ptr.tp->ptr.hp->ptr.hp->atom = 'b';
  C->ptr.tp->ptr.hp->ptr.tp = (Glist)malloc(sizeof(Glist));
  // 存放子表(c,d)，表头为c，表尾为d
  C->ptr.tp->ptr.hp->ptr.tp->tag = 1;
  C->ptr.tp->ptr.hp->ptr.tp->ptr.hp = (Glist)malloc(sizeof(Glist));
  C->ptr.tp->ptr.hp->ptr.tp->ptr.hp->tag = 0;
  C->ptr.tp->ptr.hp->ptr.tp->ptr.hp->atom = 'c';
  C->ptr.tp->ptr.hp->ptr.tp->ptr.tp = (Glist)malloc(sizeof(Glist));
  // 存放表尾d
  C->ptr.tp->ptr.hp->ptr.tp->ptr.tp->tag = 1;
  C->ptr.tp->ptr.hp->ptr.tp->ptr.tp->ptr.hp = (Glist)malloc(sizeof(Glist));
  C->ptr.tp->ptr.hp->ptr.tp->ptr.tp->ptr.hp->tag = 0;
  C->ptr.tp->ptr.hp->ptr.tp->ptr.tp->ptr.hp->atom = 'd';
  C->ptr.tp->ptr.hp->ptr.tp->ptr.tp->ptr.tp = NULL;

  return C;
}

int GlistLength(Glist C) {
  int Number = 0;
  Glist P = C;
  while (P) {
    Number++;
    P = P->ptr.tp;
  }
  return Number;
}

int GlistDepth(Glist C) {
  // 如果表C为空表时，直接返回长度1；
  if (!C) {
    return 1;
  }
  // 如果表C为原子时，直接返回0；
  if (C->tag == 0) {
    return 0;
  }
  int max = 0;  // 设置表C的初始长度为0；
  for (Glist pp = C; pp; pp = pp->ptr.tp) {
    int dep = GlistDepth(pp->ptr.hp);
    if (dep > max) {
      max = dep;  // 每次找到表中遍历到深度最大的表，并用max记录
    }
  }
  // 程序运行至此处，表明广义表不是空表，由于原子返回的是0，而实际长度是1，所以，此处要+1；
  return max + 1;
}

// Glist *T，等同于:struct GLNode* *T，此为二级指针，不是一级指针。在主函数中，调用此函数时，传入的是指针T的地址，而不是T 。
// 这里使用的是地址传递，而不是值传递。如果在这里使用值传递，会导致广义表T丢失结点，复制失败。
void copyGlist(Glist C, Glist *T) {
  // 如果C为空表，那么复制表直接为空表
  if (!C) {
    *T = NULL;
  } else {
    *T = (Glist)malloc(sizeof(GNode));  // C不是空表，给T申请内存空间
    // 申请失败，程序停止
    if (!*T) {
      exit(0);
    }
    (*T)->tag = C->tag;  // 复制表C的tag值
    // 判断当前表元素是否为原子，如果是，直接复制
    if (C->tag == 0) {
      (*T)->atom = C->atom;
    } else {  // 运行到这，说明复制的是子表
      copyGlist(C->ptr.hp, &((*T)->ptr.hp));  // 复制表头
      copyGlist(C->ptr.tp, &((*T)->ptr.tp));  // 复制表尾
    }
  }
}
#else
// 如果认为上面的存储广义表的方式不合理，广义表还有另一种存储结构
typedef struct GLNode {
  int tag;  // 标志域
  union {
    int atom;           // 原子结点的值域
    struct GLNode *hp;  // 子表结点的指针域，hp指向表头
  };
  struct GLNode *tp;  // 这里的tp相当于链表的next指针，用于指向下一个数据元素
} * Glist;

Glist creatGlist(Glist C) {
  C = (Glist)malloc(sizeof(Glist));
  C->tag = 1;
  C->hp = (Glist)malloc(sizeof(Glist));
  C->tp = NULL;
  // 表头原子a
  C->hp->tag = 0;
  C->atom = 'a';
  C->hp->tp = (Glist)malloc(sizeof(Glist));
  C->hp->tp->tag = 1;
  C->hp->tp->hp = (Glist)malloc(sizeof(Glist));
  C->hp->tp->tp = NULL;
  // 原子b
  C->hp->tp->hp->tag = 0;
  C->hp->tp->hp->atom = 'b';
  C->hp->tp->hp->tp = (Glist)malloc(sizeof(Glist));
  // 原子c
  C->hp->tp->hp->tp->tag = 0;
  C->hp->tp->hp->tp->atom = 'c';
  C->hp->tp->hp->tp->tp = (Glist)malloc(sizeof(Glist));
  // 原子d
  C->hp->tp->hp->tp->tp->tag = 0;
  C->hp->tp->hp->tp->tp->atom = 'd';
  C->hp->tp->hp->tp->tp->tp = NULL;
  return C;
}
#endif

int main() {
  Glist C = NULL;
  C = creatGlist(C);
  printf("广义表的长度为：%d\n", GlistLength(C));  // 2
  printf("广义表的深度为：%d\n", GlistDepth(C));   // 2

  Glist T = NULL;
  copyGlist(C, &T);
  printf("%c\n", T->ptr.hp->atom);  // a

  return 0;
}
