#include <iostream>
#include <string>

// 理解临时对象的来源

template <class T>
void swap(T& object1, T& object2) {
  T temp = object1;
  object1 = object2;
  object2 = temp;
  // 经常把仅仅需要一小段时间的变量称为临时变量；
  // 就C++而言，temp根本不是临时变量，它只是一个函数的局部对象。
}

// 在C++中真正的临时对象是看不见的，它们不出现在源代码中。
// 建立一个没有命名的非堆（non-heap）对象会产生临时对象。
// 这种未命名的对象通常在两种条件下产生：
// 1.为了使函数成功调用而进行隐式类型转换；
// 2.函数返回对象时。
// 理解如何和为什么建立这些临时对象是很重要的，
// 因为构造和释放它们的开销对于程序的性能来说有着不可忽视的影响。

namespace n1 {
// 首先考虑为使函数成功调用而建立临时对象的情况，
// 当传送给函数的对象类型与参数类型不匹配时会发生。
size_t countChar(const std::string& str, char ch) { return 5; }
// 这样的类型转换很方便（尽管很危险-条款M5），但是从效率的观点来看，
// 临时string对象的构造和释放是不必要的开销，通常有两个方法可以消除它：
// 1.一种是重新设计代码，不让发生这种类型转换，参考条款M5；
// 2.另一种方法是通过修改软件而不再需要类型转换，参考条款M21。
// 仅当通过传值方式传递对象或传递常量引用参数时，才会发生这些类型转换。
void func1() {
  char buffer[10];
  char c;
  countChar(buffer, c);
  // 第一个被传送的参数是字符数组，但是对应函数的参数的类型是const string&。
  // 仅当消除类型不匹配后，才能成功进行这个调用，
  // 编译器很乐意消除它，方法是建立一个string类型的临时对象。
  // 通过以buffer做为参数调用string的构造函数来初始化这个临时对象。
  // countChar的参数str被绑定在这个临时的string对象上，
  // 当countChar返回时，临时对象自动释放。
}

// 当传递一个非常量引用参数对象，就不会发生：
void uppercasify(std::string& str) {}
// 当程序员期望修改非临时对象时，对非常量引用进行的隐式类型转换却修改临时对象。
// 这就是为什么C++语言禁止为非常量引用产生临时对象，
// 这样非常量引用（reference-to-non-const）参数就不会遇到这种问题。
void func2() {
  // 在字符计数的例子里，能够成功传递char数组到countChar中，
  // 但是试图用char数组调用upeercasify函数，则不会成功：
  char subtleBookPlug[] = "Effective C++";
  // uppercasify(subtleBookPlug);  // 错误!
  // 假设建立一个临时对象，那么临时对象将被传递到upeercasify中，
  // 其会修改这个临时对象，把它的字符改成大写。
  // 但是对subtleBookPlug函数调用的真正参数没有任何影响；
  // 仅仅改变了临时从subtleBookPlug生成的string对象，无疑这不是所希望的，
  // 传递subtleBookPlug参数到uppercasify函数中，期望修改subtleBookPlug的值。
}
}  // namespace n1

namespace n2 {
// 建立临时对象的第二种环境是函数返回对象时。
// const Number operator+(const Number& lhs, const Number& rhs);
// 这个函数的返回值是临时的，因为它没有被命名，它只是函数的返回值。
// 必须为每次调用operator+构造和释放这个对象而付出代价，通常不想付出这样的开销。
// 对于这种函数，可以切换到operator=，而避免开销。
// 不过对于大多数返回对象的函数来说，无法切换到不同的函数，
// 从而没有办法避免构造和释放返回值。
// 至少在概念上没有办法避免它，然而概念和现实之间又一个黑暗地带，
// 叫做优化，有时能以某种方法编写返回对象的函数，以允许你的编译器优化临时对象。
// 这些优化中，最常见和最有效的是返回值优化，条款M20。

// 综上所述，临时对象是有开销的，所以应该尽可能地去除它们，
// 然而更重要的是训练自己寻找可能建立临时对象的地方。
// 在任何时候只要见到常量引用（reference-to-const）参数，
// 就存在建立临时对象而绑定在参数上的可能性。
// 在任何时候只要见到函数返回对象，就会有一个临时对象被建立（以后被释放）。
// 学会寻找这些对象构造，就能强透过编译器表面动作而看到其背后的开销。
}  // namespace n2

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func1();
      break;
    case 1:
      n1::func2();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}