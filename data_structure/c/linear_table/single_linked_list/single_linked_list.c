#include "single_linked_list.h"

// 头节点，头指针和首元节点。一个完整的链表需要由以下几部分构成：
// 1.头指针：一个普通的指针，它的特点是永远指向链表第一个节点的位置。
//   很明显，头指针用于指明链表的位置，便于后期找到链表并使用表中的数据；
// 2.节点：链表中的节点又细分为头节点、首元节点和其他节点：
//   a.头节点：其实就是一个不存任何数据的空节点，通常作为链表的第一个节点。
//     对于链表来说，头节点不是必须的，它的作用只是为了方便解决某些实际问题；
//   b.首元节点：由于头节点（也就是空节点）的缘故，链表中称第一个存有数据的节点为首元节点。
//     首元节点只是对链表中第一个存有数据节点的一个称谓，没有实际意义；
//   c.其他节点：链表中其他的节点；
//   链表中有头节点时，头指针指向头节点；
//   反之，若链表中没有头节点，则头指针指向首元节点。

// 链表的创建（初始化）
// 1.声明一个头指针（如果有必要，可以声明一个头节点）；
// 2.创建多个存储数据的节点，在创建的过程中，要随时与其前驱节点建立逻辑关系；

// 无头节点的链表:
link* initLinkNoHead() {
  link* p = NULL;                            // 创建头指针
  link* temp = (link*)malloc(sizeof(link));  // 创建首元节点
  // 首元节点先初始化
  temp->elem = 1;
  temp->next = NULL;
  p = temp;  // 头指针指向首元节点
  // 从第二个节点开始创建
  for (int i = 2; i < 5; i++) {
    // 创建一个新节点并初始化
    link* a = (link*)malloc(sizeof(link));
    a->elem = i;
    a->next = NULL;
    // 将temp节点与新建立的a节点建立逻辑关系
    temp->next = a;
    // 指针temp每次都指向新链表的最后一个节点，其实就是a节点，这里写temp=a也对
    temp = temp->next;  // temp = a;
  }
  // 返回建立的节点，只返回头指针p即可，通过头指针即可找到整个链表
  return p;
}
// 含头节点的链表
link* initLinkWithHead() {
  link* p = (link*)malloc(sizeof(link));  // 创建一个头结点
  link* temp = p;                         // 声明一个指针指向头结点
  // 生成链表
  for (int i = 1; i < 5; i++) {
    link* a = (link*)malloc(sizeof(link));
    a->elem = i;
    a->next = NULL;
    temp->next = a;
    temp = temp->next;
  }
  return p;
}

void displayNoHead(link* p) {
  link* temp = p;  // 将temp指针重新指向头结点
  // 只要temp指针指向的结点的next不是NULL，就执行输出语句。
  while (temp) {
    printf("%d ", temp->elem);
    temp = temp->next;
  }
  printf("\n");
}
void displayWithHead(link* p) {
  link* temp = p;  // 将temp指针重新指向头结点
  // 只要temp指针指向的结点的next不是NULL，就执行输出语句。
  while (temp->next) {
    temp = temp->next;
    printf("%d ", temp->elem);
  }
  printf("\n");
}

// 链表插入元素
// 同顺序表一样，向链表中增添元素，根据添加位置不同，可分为以下3种情况：
// 1.插入到链表的头部（头节点之后），作为首元节点；
// 2.插入到链表中间的某个位置；
// 3.插入到链表的最末端，作为链表中最后一个数据元素；
// 虽然新元素的插入位置不固定，但是链表插入元素的思想是固定的，
// 只需做以下两步操作，即可将新元素插入到指定的位置：
// 1.将新结点的next指针指向插入位置后的结点；
// 2.将插入位置前结点的next指针指向插入结点；
// 链表插入元素的操作必须是先步骤1，再步骤2；
// 反之，若先执行步骤2，除非再添加一个指针，作为插入位置后续链表的头指针，
// 否则会导致插入位置后的这部分链表丢失，无法再实现步骤1。

// p为原链表，elem表示新数据元素，add表示新元素要插入的位置
link* insertElem(link* p, int elem, int add) {
  link* temp = p;  // 创建临时结点temp
  // 首先找到要插入位置的上一个结点
  for (int i = 1; i < add; i++) {
    temp = temp->next;
    if (temp == NULL) {
      printf("插入位置无效\n");
      return p;
    }
  }
  // 创建插入结点c
  link* c = (link*)malloc(sizeof(link));
  c->elem = elem;
  // 向链表中插入结点
  c->next = temp->next;
  temp->next = c;
  return p;
}

// 链表删除元素
// 从链表中删除指定数据元素时，实则就是将存有该数据元素的节点从链表中摘除，
// 但作为一名合格的程序员，要对存储空间负责，对不再利用的存储空间要及时释放。
// 因此，从链表中删除数据元素需要进行以下2步操作：
// 1.将结点从链表中摘下来;
// 2.手动释放掉结点，回收被结点占用的存储空间;
// 其中，从链表上摘除某节点的实现非常简单，只需找到该节点的直接前驱节点temp，执行一行程序：
// temp->next=temp->next->next;
// p为原链表，add为要删除元素的值
link* delElem(link* p, int add) {
  link* temp = p;
  // 遍历到被删除结点的上一个结点
  for (int i = 1; i < add; i++) {
    temp = temp->next;
    if (temp->next == NULL) {
      printf("没有该结点\n");
      return p;
    }
  }
  link* del = temp->next;  // 单独设置一个指针指向被删除结点，以防丢失
  // 删除某个结点的方法就是更改前一个结点的指针域
  temp->next = temp->next->next;
  free(del);  // 手动释放该结点，防止内存泄漏
  return p;
}

// 链表查找元素
// 在链表中查找指定数据元素，最常用的方法是：
// 从表头依次遍历表中节点，用被查找元素与各节点数据域中存储的数据元素进行比对，
// 直至比对成功或遍历至链表最末端的NULL（比对失败的标志）。

// p为原链表，elem表示被查找元素
int selectElem(link* p, int elem) {
  // 新建一个指针t，初始化为头指针p
  link* t = p;
  int i = 1;
  // 由于头节点的存在，因此while中的判断为t->next
  while (t->next) {
    t = t->next;
    if (t->elem == elem) {
      return i;
    }
    i++;
  }
  // 程序执行至此处，表示查找失败
  return -1;
}
// 注意，遍历有头节点的链表时，需避免头节点对测试数据的影响，因此在遍历链表时，
// 建议使用上面代码中的遍历方法，直接越过头节点对链表进行有效遍历。

// 链表更新元素
// 更新函数，其中，add表示更改结点在链表中的位置，newElem为新的数据域的值
link* amendElem(link* p, int add, int newElem) {
  link* temp = p;
  temp = temp->next;  // 在遍历之前，temp指向首元结点
  // 遍历到待更新结点
  for (int i = 1; i < add; i++) {
    temp = temp->next;
  }
  temp->elem = newElem;
  return p;
}
