#include <iostream>

using namespace std;

// 类模板中可以定义静态成员，从该类模板实例化得到的所有类都包含同样的静态成员。

template <class T>
class A {
 private:
  static int count;

 public:
  A() { count++; }
  ~A() { count--; };
  A(A &) { count++; }
  static void PrintCount() { cout << count << endl; }
};
// 对静态成员变量在类外部加以声明是必需的。
// A<int>和A<double>是两个不同的类。虽然它们都有静态成员变量count，但是显然，A<int>的对象ia和 <double>的对象da不会共享一份count。
template <>
int A<int>::count = 0;
template <>
int A<double>::count = 0;

int main() {
  A<int> ia;
  A<double> da;
  ia.PrintCount();  // 1
  da.PrintCount();  // 1

  return 0;
}