#include <iostream>

// 绝不在构造和析构函数中调用virtual函数

// base class构造期间virtual函数不会调用derived class版本。
// 可以理解为，在base class构造期间，virtual函数不是virtual函数。
// 当base class构造函数执行时，derived class的成员变量尚未初始化。
// 如果virtual函数调用derived class的版本，行为未定义。
// 析构函数也是相同的道理，derived class析构函数开始执行，
// 对象内的derived class成员变量是未定义的。
// base class析构函数调用base class版本virtual函数。
class Base {
 public:
  Base() { display(); }
  ~Base() { display(); }
  virtual void display() { std::cout << "Base display" << std::endl; }
};
class Derived : public Base {
  virtual void display() { std::cout << "Derived display" << std::endl; }
};

// 请记住：
// 1.在基类构造和析构函数中不要调用virtual函数，
//   因为这类调用从不会调用到derived class中的函数。

int main() {
  Base *b = new Derived();
  // Base display

  delete b;
  // Base display

  return 0;
}