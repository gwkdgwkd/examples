#ifndef _STATIC_LINKED_LIST_
#define _STATIC_LINKED_LIST_

#include <stdio.h>

#define maxSize 7

// 静态链表存储数据元素也需要自定义数据类型，至少需要包含以下2部分信息：
// 1.数据域：用于存储数据元素的值；
// 2.游标：其实就是数组下标，表示直接后继元素所在数组中的位置；
typedef struct {
  int data;  // 数据域
  int next;   // 游标
} component;

// 将结构体数组中所有分量链接到备用链表中
void reserveArr(component *array);
// 初始化静态链表
int initArr(component *array);
// 向链表中插入数据，body表示链表的头结点在数组中的位置，
// add表示插入元素的位置，a表示要插入的数据
void insertArr(component *array, int body, int add, char a);
// 删除链表中含有字符a的结点
void deletArr(component *array, int body, char a);
// 查找存储有字符elem的结点在数组的位置
int selectElem(component *array, int body, char elem);
// 将链表中的字符oldElem改为newElem
void amendElem(component *array, int body, char oldElem, char newElem);
// 输出函数
void displayArr(component *array, int body);
// 从备用链表中摘除空闲节点的实现函数
int mallocArr(component *array);
// 将摘除下来的节点链接到备用链表上
void freeArr(component *array, int k);

#endif  // _STATIC_LINKED_LIST_