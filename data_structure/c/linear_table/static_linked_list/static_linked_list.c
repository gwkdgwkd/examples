#include "static_linked_list.h"

// 是否存在一种存储结构，可以融合顺序表和链表各自的优点，从而既能快速访问元素，又能快速增加或删除数据元素。
// 静态链表，也是线性存储结构的一种，它兼顾了顺序表和链表的优点于一身，可以看做是顺序表和链表的升级版。
// 使用静态链表存储数据，数据全部存储在数组中（和顺序表一样），但存储位置是随机的，数据之间"一对一"的逻辑关系通过一个整形变量（称为"游标"，和指针功能类似）维持（和链表类似）。
// 静态链表中，除了数据本身通过游标组成的链表外，还需要有一条连接各个空闲位置的链表，称为备用链表。
// 备用链表的作用是回收数组中未使用或之前使用过（目前未使用）的存储空间，留待后期使用。
// 也就是说，静态链表使用数组申请的物理空间中，存有两个链表，一条连接数据，另一条连接数组中未使用的空间。
// 静态链表中设置备用链表的好处是，可以清楚地知道数组中是否有空闲位置，以便数据链表添加新数据时使用。

// 静态链表和动态链表的共同点是，数据之间"一对一"的逻辑关系都是依靠指针（静态链表中称"游标"）来维持，仅此而已。
// 静态链表
//  使用静态链表存储数据，需要预先申请足够大的一整块内存空间，也就是说，静态链表存储数据元素的个数从其创建的那一刻就已经确定，后期无法更改。
//  比如，如果创建静态链表时只申请存储10个数据元素的空间，那么在使用静态链表时，数据的存储个数就不能超过10个，否则程序就会发生错误。
//  不仅如此，静态链表是在固定大小的存储空间内随机存储各个数据元素，这就造成了静态链表中需要使用另一条链表（通常称为"备用链表"）来记录空间存储空间的位置，以便后期分配给新添加元素使用。
//  这意味着，如果你选择使用静态链表存储数据，你需要通过操控两条链表，一条是存储数据，另一条是记录空闲空间的位置。
// 动态链表
//  使用动态链表存储数据，不需要预先申请内存空间，而是在需要的时候才向内存申请。也就是说，动态链表存储数据元素的个数是不限的，想存多少就存多少。
//  同时，使用动态链表的整个过程，你也只需操控一条存储数据的链表。当表中添加或删除数据元素时，你只需要通过malloc或free函数来申请或释放空间即可，实现起来比较简单。

// 创建备用链表
void reserveArr(component *array) {
  for (int i = 0; i < maxSize; i++) {
    array[i].cur = i + 1;  // 将每个数组分量链接到一起
    array[i].data = ' ';
  }
  array[maxSize - 1].cur = 0;  // 链表最后一个结点的游标值为0
}

// 提取分配空间
int mallocArr(component *array) {
  // 若备用链表非空，则返回分配的结点下标，否则返回0（当分配最后一个结点时，该结点的游标值为0）
  int i = array[0].cur;
  if (array[0].cur) {
    array[0].cur = array[i].cur;
  }
  return i;
}

// 初始化静态链表
int initArr(component *array) {
  reserveArr(array);
  int body = mallocArr(array);
  // 声明一个变量，把它当指针使，指向链表的最后的一个结点，因为链表为空，所以和头结点重合
  int tempBody = body;
  for (int i = 1; i < 5; i++) {
    int j = mallocArr(array);  // 从备用链表中拿出空闲的分量
    array[tempBody].cur = j;  // 将申请的空线分量链接在链表的最后一个结点后面
    array[j].data = 'a' + i - 1;  // 给新申请的分量的数据域初始化
    tempBody = j;  // 将指向链表最后一个结点的指针后移
  }
  array[tempBody].cur = 0;  // 新的链表最后一个结点的指针设置为0
  return body;
}

void displayArr(component *array, int body) {
  int tempBody = body;  // tempBody准备做遍历使用
  while (array[tempBody].cur) {
    printf("%c,%d ", array[tempBody].data, array[tempBody].cur);
    tempBody = array[tempBody].cur;
  }
  printf("%c,%d\n", array[tempBody].data, array[tempBody].cur);
}

// 静态链表添加元素
// 向链表中插入数据，body表示链表的头结点在数组中的位置，add表示插入元素的位置，a表示要插入的数据
void insertArr(component *array, int body, int add, char a) {
  int tempBody = body;  // tempBody做遍历结构体数组使用
  // 找到要插入位置的上一个结点在数组中的位置
  for (int i = 1; i < add; i++) {
    tempBody = array[tempBody].cur;
  }
  int insert = mallocArr(array);  // 申请空间，准备插入
  array[insert].data = a;
  array[insert].cur =
      array[tempBody].cur;  // 新插入结点的游标等于其直接前驱结点的游标
  array[tempBody].cur =
      insert;  // 直接前驱结点的游标等于新插入结点所在数组中的下标
}

// 静态链表删除元素
// 静态链表中删除指定元素，只需实现以下 2 步操作：
//  将存有目标元素的节点从数据链表中摘除；
//  将摘除节点添加到备用链表，以便下次再用；
// 提示：若问题中涉及大量删除元素的操作，建议读者在建立静态链表之初创建一个带有头节点的静态链表，方便实现删除链表中第一个数据元素的操作。
// 备用链表回收空间的函数，其中array为存储数据的数组，k表示未使用节点所在数组的下标
void freeArr(component *array, int k) {
  array[k].cur = array[0].cur;
  array[0].cur = k;
}
// 删除结点函数，a表示被删除结点中数据域存放的数据
void deletArr(component *array, int body, char a) {
  int tempBody = body;
  // 找到被删除结点的位置
  while (array[tempBody].data != a) {
    tempBody = array[tempBody].cur;
    // 当tempBody为0时，表示链表遍历结束，说明链表中没有存储该数据的结点
    if (tempBody == 0) {
      printf("链表中没有此数据");
      return;
    }
  }
  // 运行到此，证明有该结点
  int del = tempBody;
  tempBody = body;
  // 找到该结点的上一个结点，做删除操作
  while (array[tempBody].cur != del) {
    tempBody = array[tempBody].cur;
  }
  // 将被删除结点的游标直接给被删除结点的上一个结点
  array[tempBody].cur = array[del].cur;
  // 回收被摘除节点的空间
  freeArr(array, del);
}

// 静态链表查找元素
// 静态链表查找指定元素，由于我们只知道静态链表第一个元素所在数组中的位置，因此只能通过逐个遍历静态链表的方式，查找存有指定数据元素的节点。
// 在以body作为头结点的链表中查找数据域为elem的结点在数组中的位置
int selectElem(component *array, int body, char elem) {
  int tempBody = body;
  // 当游标值为0时，表示链表结束
  while (array[tempBody].cur != 0) {
    if (array[tempBody].data == elem) {
      return tempBody;
    }
    tempBody = array[tempBody].cur;
  }
  return -1;  // 返回-1，表示在链表中没有找到该元素
}

// 静态链表中更改数据
// 更改静态链表中的数据，只需找到目标元素所在的节点，直接更改节点中的数据域即可。
// 在以body作为头结点的链表中将数据域为oldElem的结点，数据域改为newElem
void amendElem(component *array, int body, char oldElem, char newElem) {
  int add = selectElem(array, body, oldElem);
  if (add == -1) {
    printf("无更改元素");
    return;
  }
  array[add].data = newElem;
}