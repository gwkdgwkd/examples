#include <iostream>

// C++11扩展的friend语法：
// 1.声明一个类为另外一个类的友元时，不再需要使用class关键字；
// 2.声明友元类时，甚至可以使用类的别名。

class Poly;
typedef Poly P;

class LiLei {
  friend class Poly;  // C++98通过，C++11通过
};

class Jim {
  friend Poly;  // C++98失败，C++11通过
};

class HanMeiMei {
  friend P;  // C++98失败，C++11通过
};
// C++98也能编译过，为什么？编译器比较新？

int main(int argc, char* argv[]) { return 0; }