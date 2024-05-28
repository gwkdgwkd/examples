#include <iostream>

// 所谓野指针，又称悬挂指针，指的是没有明确指向的指针。
// 野指针往往指向的是那些不可用的内存区域，
// 这就意味着像操作普通指针那样使用野指针（例如&p），极可能导致程序发生异常。
// 实际开发中，避免产生野指针最有效的方法，就是在定义指针的同时完成初始化操作，
// 即便该指针的指向尚未明确，也要将其初始化为空指针。

// C++98/03标准中，将一个指针初始化为空指针的方式有2种：
// int *p = 0;
// int *p = NULL;  // 推荐使用
// 将指针明确指向0（0x0000 0000）这个内存空间，一方面可以避免其成为野指针；
// 另一方面，大多数操作系统都不允许用户对地址为0的内存空间执行写操作，
// 若用户尝试修改其内容，则程序运行会直接报错。
// 相比第一种方式，更习惯将指针初始化为NULL，NULL并不是C++的关键字，
// 它是C++为事先定义好的一个宏，并且它的值往往就是字面量0（#define NULL 0）。

// C++中将NULL定义为字面常量0，虽然能满足大部分场景的需要，
// 但个别情况下，它会导致程序的运行和预期不符：
void isnull(void *c) { std::cout << "void *c" << std::endl; }
void isnull(int n) { std::cout << "int n" << std::endl; }

int main() {
  // 对于isnull(0)来说，显然它真正调用的是参数为整形的isnull()函数；
  // 而对于isnull(NULL)，期望它实际调用的是参数为void *c的isnull()函数。
  isnull(0);  // int n
  // isnull(NULL);  // int n
  // call of overloaded ‘isnull(NULL)’ is ambiguous

  // C++98/03标准中，如果想令isnull(NULL)实际调用的是isnull(void* c)，
  // 就需要对NULL（或者0）进行强制类型转换：
  isnull((void *)NULL);  // void *c
  isnull((void *)0);     // void *c

  // 由于C++98标准使用期间，NULL已经得到了广泛的应用，
  // 出于兼容性的考虑，C++11标准并没有对NULL的宏定义做任何修改。
  // 为了修正C++存在的这一BUG，C++标准委员会最终决定另其炉灶，
  // 在C++11标准中引入一个新关键字，即nullptr。

  // nullptr是nullptr_t类型的右值常量，专用于初始化空类型指针。
  // nullptr_t是C++11新增加的数据类型，可称为指针空值类型。
  // 也就是说，nullptr仅是该类型的一个实例对象，已经定义好，
  // 可以直接使用，如果需要完全定义出多个同nullptr完全一样的实例对象。

  // nullptr可以被隐式转换成任意的指针类型：
  int *a1 = nullptr;
  char *a2 = nullptr;
  double *a3 = nullptr;
  // 不同类型的指针变量都可以使用nullptr来初始化，
  // 编译器分别将nullptr隐式转换成int*、char*以及double*指针类型。

  // 通过将指针初始化为nullptr，可以很好地解决NULL遗留的问题：
  // isnull(NULL);  // int n
  isnull(nullptr);  // void *c
  // 由于nullptr无法隐式转换为整形，而可以隐式匹配指针类型，结果符合预期。

  // 在C++11中，相比NULL和0，使用nullptr初始化空指针可以令程序更加健壮。

  return 0;
}