#include <iostream>
#include <vector>
#include "template.h"

// 在标准C++中，只要在编译单元内遇到被完整定义的模板，编译器都必须将其实例化。
// 这会大大增加编译时间，特别是模板在许多编译单元内使用相同的参数实例化。
// 而且没有办法告诉C++不要引发模板的实例化。
// C++已经有了强制编译器在特定位置开始实例化的语法（显式实例化）：
template class std::vector<int>;
// 但C++没有阻止编译器在某个编译单元内实例化模板的能力，C++11引入外部模板这一概念，将语法扩展如下：
extern template class std::vector<int>;
// 这样就告诉编译器不要在该编译单元内将该模板实例化，避免编译器进行大量冗余工作。

// 外部“extern”这个概念在C中就有了，在一个文件a.c中定义了一个变量int i，
// 要在另外一个文件b.c中使用它，这个时候就会在b.c中做一个外部变量的声明，例如：
extern int i;
// 这样做的好处是在分别编译了a.c和b.c之后，它们生成的目标文件a.o和b.o中只有i这个符号的一份定义。
// 具体说就是a.o中的i实际存在于a.o目标文件的数据区中的数据，
// 而在b.o中只是记录了i会引用其他目标文件数据区中名为i的数据。
// 在链接器将a.o和b.o链接成一个可执行文件c的时候，c的数据区中也只有一个i的数据。
// 如果在b.c中声明i时不叫extern，那么i既存在于a.o的数据区中，也存在于b.o的数据区中。
// 在链接器链接a.o和b.o的时候，就会报错，因为无法决定相同的符号是否需要合并。

// 相似的对于函数模板也会遇到同样的问题，区别是一个是变量，而另一个是函数。问题出现在模板的实例化。
// test.h
// template <typename T>
// void fun(T) { }
// test1.cpp
// #include "test.h"
// void test1() { fun(1); }
// test2.cpp
// #include "test.h"
// void test2() { fun(2); }
// 在编译test1.cpp的时候，编译器实例化出了函数fun<int>(int)，在编译test2.cpp的时候，
// 又实例化出了函数fun<int>(int)。
// 所以在目标文件test1.o和test2.o中，就会有两份相同的函数fun<int>(int)代码。
// 代码重复，为了节省空间，值保留其中一个。
// 在链接的时候，链接器会通过一些手段，将重复的的函数模板fun<int>(int)删掉，
// 只保留一个，这样就解决了模板实例化产生的代码冗余问题。
// 编译器要对每一处模板进行实例化，链接器还要移除重复的实例化代码。
// 当在广泛使用模板的项目中，编译器会产生大量的冗余代码，这会极大的增加编译时间和链接时间。
// 解决这个问题的方法和全局变量的一样，就是使用外部模板。
// 外部模板依赖于C++98中的一个已有特性，显式实例化:
// test1.cpp
// #include "test.h"
// template void fun<int>(int);  // 显式地实例化
// void test1() { fun(1); }
// test2.cpp, 这样在test2.o中就不再生成fun<int>(int)实例化代码。
// #include "test.h"
// extern template void fun<int>(int);  // 外部模板的声明
// void test2() { fun(2); }
// 编译器不用每次实例化都产生一份fun<int>(int)的代码，减少编译时间。
// 链接器的工作也很轻松，test1.cpp和test2.cpp共享一份代码。

// 注意问题:
// 如果外部模板声明出现在某个编译单元中，
// 那么与之对应的显示实例化必须出现在另一个编译单元中或者同一个编译单元的后续代码中。
// 外部模板不能用于一个静态函数，但可以用于类静态成员函数。
// 因为静态函数没有外部链接属性，不能在本编译单元之外出现。

extern template class A<int>;
extern template void B<int>::f_m(int);
// extern template void f1<int>(int);  // error: ‘void f1<int>(int)’未定义的引用
extern template void f2<int>(int);

void f() {
  f2(1);
  std::cout << "f4" << std::endl;
}

int main() {
  f1(1);
  f();
  B<int>::f_m(1);

  // static f1
  // f2
  // f4
  // B::f_m t: 1

  return 0;
}