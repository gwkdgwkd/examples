#include <iostream>
#include <string>

using namespace std;

// 了解C++默默编写并调用哪些函数

// 什么时候空类不再是个空类呢？当C++处理之后。
// 如果没有声明，编译器会声明一个copy构造函数、一个copy assignment操作符和一个析构函数（non-virtual）。
// 如果没有声明任何构造函数，编译器也会声明一个default构造函数。所有这些函数都是public且inline的。
// class Empty{ }; 等价于：
class Empty {
 public:
  Empty() {}
  Empty(const Empty& rhs) {}
  ~Empty() {}
  Empty& operator=(const Empty& rhs) {}
};

// 唯有当这些函数被需要（被调用），它们才会被编译器创建出来。
// default构造函数和析构函数主要是给编译器一个地方用来放置“藏身幕后”的代码，像调用base class和non-static成员变量的构造函数和析构函数。
// copy构造函数和copy assignment操作符，编译器创建的版本只是单纯地将来源对象的每一个non-static成员变量拷贝到目标对象。
// 编译器生成的copy assignment操作符行为基本上与copy构造函数如出一辙，但一般而言只有当生出的代码合法且有适当机会证明它有意义，才会产生。
template <class T>
class NamedObject {
 public:
  NamedObject(string& name, const T& value);

 private:
  // 1 C++并不允许让引用改指向不同的对象。如果打算在一个内含引用成员的class内支持赋值操作，必须自己定义copy assignment操作符。
  string& nameValue;
  // 2 内含const成员也一样，更改const成员是不合法的，所以编译器不知道如何生成默认的赋值函数面对它们。也要自己定义。
  const T objectValue;
};
// 3 如果某个base class将copy assignment操作符声明为private，编译器拒绝为其derived class生成一个copy assignment操作符。
//   毕竟编译器为derived class所生成订的copy assignment操作符想象中可以处理base class成分，但当无法调用时，就不生成，也就不允许赋值操作。
class Base {
 private:
  Empty& operator=(const Base& rhs) {}
};
class Derived : public Base {};

// 请记住：
// 编译器可以暗自为class创建default构造函数、copy构造函数、copy assignment操作符，以及析构函数。

int main() {
  // 下面的代码会造成每个默认函数都被编译器产出：
  Empty e1;
  Empty e2(e1);
  e2 = e1;

  string newDog("P");
  string oldDog("Q");
  NamedObject<int> p(newDog, 2);
  NamedObject<int> q(oldDog, 36);
  // p = q;  // C++并不允许让引用改指向不同的对象。

  Derived a, b;
  // a = b;

  return 0;
}