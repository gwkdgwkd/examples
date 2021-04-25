#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 串存储结构，也就是存储字符串的数据结构。
// 很明显，字符串之间的逻辑关系也是“一对一”，用线性表的思维不难想出，串存储结构也有顺序存储和链式存储。
// 提到字符串，常做的操作就是串之间的匹配，2种串的模式匹配算法，BF算法和KMP算法。

// 严格意义上讲，串存储结构也是一种线性存储结构，因为字符串中的字符之间也具有"一对一"的逻辑关系。只不过，与之前所学的线性存储结构不同，串结构只用于存储字符类型的数据。
// 数据结构中，根据串中存储字符的数量及特点，对一些特殊的串进行了命名，比如说：
//  空串：存储0个字符的串，例如S = ""（双引号紧挨着）；
//  空格串：只包含空格字符的串，例如S = "     "（双引号包含5个空格）；
//  子串和主串：假设有两个串a和b，如果a中可以找到几个连续字符组成的串与b完全相同，则称a是b的主串，b是a的子串。
//  例如，若a="shujujiegou"，b="shuju"，由于a中也包含"shuju"，因此串a和串b是主串和子串的关系；

// 存储一个字符串，数据结构包含以下 3 种具体存储结构：
//  定长顺序存储：实际上就是用普通数组（又称静态数组）存储。例如C语言使用普通数据存储字符串的代码为char a[20] = "data.biancheng.net"；
//  堆分配存储：用动态数组存储字符串；
//  块链存储：用链表存储字符串；

// 串的定长顺序存储结构
// 顺序存储结构（顺序表）的底层实现用的是数组，根据创建方式的不同，数组又可分为静态数组和动态数组，因此顺序存储结构的具体实现其实有两种方式。
// 串的定长顺序存储结构，可以简单地理解为采用 "固定长度的顺序存储结构" 来存储字符串，因此限定了其底层实现只能使用静态数组。
// 例如，采用定长顺序存储结构存储 "data.biancheng.net"，通过目测得知此字符串长度为18，因此我们申请的数组空间长度至少为19（最后一位存储字符串的结束标志'\0'）。

// 串的堆分配存储结构
// 通常，编程语言会将程序占有的内存空间分成多个不同的区域，程序包含的数据会被分门别类并存储到对应的区域。拿C语言来说，程序会将内存分为4个区域，分别为堆区、栈区、数据区和代码区。
// 与其他区域不同，堆区的内存空间需要程序员手动使用malloc函数申请，并且在不用后要手动通过free函数将其释放。
// C语言中使用malloc函数最多的场景是给数组分配空间，这类数组称为动态数组。
// 动态数组相比普通数组（静态数组）的优势是长度可变，换句话说，根据需要动态数组可额外申请更多的堆空间（使用relloc函数）。

// 串的块链存储结构
// 实现串的块链存储使用的是无头节点的单链表。当然根据实际需要，你也可以自行决定所用链表的结构（双向链表还是单链表，有无头节点）。
// 链表各节点存储数据个数的多少可参考以下几个因素：
//  串的长度和存储空间的大小：若串包含数据量很大，且链表申请的存储空间有限，此时应尽可能的让各节点存储更多的数据，提高空间的利用率（每多一个节点，就要多申请一个指针域的空间）；
//                        反之，如果串不是特别长，或者存储空间足够，就需要再结合其他因素综合考虑；
//  程序实现的功能：如果实际场景中需要对存储的串做大量的插入或删除操作，则应尽可能减少各节点存储数据的数量；反之，就需要再结合其他因素。

#define linkNum 3  // 全局设置链表中节点存储数据的个数
typedef struct Link {
  char a[linkNum];    // 数据域可存放 linkNum 个数据
  struct Link* next;  // 代表指针域，指向直接后继元素
} link;               // nk为节点名，每个节点都是一个link结构体
// 初始化链表，其中head为头指针，str为存储的字符串
link* initLink(link* head, char* str) {
  int length = strlen(str);
  // 根据字符串的长度，计算出链表中使用节点的个数
  int num = length / linkNum;
  if (length % linkNum) {
    num++;
  }
  // 创建并初始化首元节点
  head = (link*)malloc(sizeof(link));
  head->next = NULL;
  link* temp = head;
  // 初始化链表
  for (int i = 0; i < num; i++) {
    int j = 0;
    for (; j < linkNum; j++) {
      if (i * linkNum + j < length) {
        temp->a[j] = str[i * linkNum + j];
      } else
        temp->a[j] = '#';
    }
    if (i * linkNum + j < length) {
      link* newlink = (link*)malloc(sizeof(link));
      newlink->next = NULL;
      temp->next = newlink;
      temp = newlink;
    }
  }
  return head;
}
//输出链表
void displayLink(link* head) {
  link* temp = head;
  while (temp) {
    for (int i = 0; i < linkNum; i++) {
      printf("%c", temp->a[i]);
    }
    temp = temp->next;
  }
  printf("\n");
}

int main() {
  char* a1 = NULL;
  char* a2 = NULL;
  a1 = (char*)malloc(10 * sizeof(char));
  strcpy(a1, "data.bian");  //　将字符串"data.bian"复制给a1
  a2 = (char*)malloc(10 * sizeof(char));
  strcpy(a2, "cheng.net");
  int lengthA1 = strlen(a1);  //　a1串的长度
  int lengthA2 = strlen(a2);  //　a2串的长度
  //　尝试将合并的串存储在a1中，如果a1空间不够，则用realloc动态申请
  if (lengthA1 < lengthA1 + lengthA2) {
    a1 = (char*)realloc(a1, (lengthA1 + lengthA2 + 1) * sizeof(char));
  }
  //　合并两个串到a1中
  for (int i = lengthA1; i < lengthA1 + lengthA2; i++) {
    a1[i] = a2[i - lengthA1];
  }
  //　串的末尾要添加\0，避免出错
  a1[lengthA1 + lengthA2] = '\0';
  printf("%s\n", a1);  // data.biancheng.net
  //　用完动态数组要立即释放
  free(a1);
  free(a2);

  link* head = NULL;
  head = initLink(head, "data.biancheng.net");
  displayLink(head);  // data.biancheng.net

  return 0;
}