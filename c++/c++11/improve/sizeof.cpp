#include <iostream>

// 从C语言被发明开始，sizeof就是一个运算符，也是C语言中除了加减乘除以外为数不多的特殊运算符之一。
// 而在C++引入类（class）类型之后，sizeof的定义也随之进行了拓展。
// 不过在C++98标准中，对非静态成员变量使用sizeof是不能够通过编译的。
// 在C++11中，对非静态成员变量使用sizeof操作是合法的。
// 而在C++98中，只有静态成员，或者对象的实例才能对其成员进行sizeof操作。

struct People {
 public:
  int hand;
  static People* all;
};

int main() {
  People p;
  std::cout << sizeof(p.hand) << std::endl;  // 4 C++98中通过，C++11中通过
  std::cout << sizeof(People::all) << std::endl;  // 8 C++98中通过，C++11中通过
  std::cout << sizeof(People::hand) << std::endl;  // 4 C++98中错误，C++11中通过

  // 如果只有一个支持C++98标准的编译器，在没有定义类实例的时候，要获得类成员的大小，以前通常会采用以下的代码：
  std::cout << sizeof(((People*)0)->hand) << std::endl;  // 4
  // 强制转换0为一个People类的指针，继而通过指针的解引用获得其成员变量，并用sizeof求得该成员变量的大小。

  // 在C++11中，无需这样的技巧，因为sizeof可以作用的表达式包括了类成员表达式。如下代码：
  std::cout << sizeof(People::hand) << std::endl;  // 4
  // 可以看到，无论从代码的可读性还是编写的便利性，C++11的规则都比强制指针转换的方案更胜一筹。

  return 0;
}