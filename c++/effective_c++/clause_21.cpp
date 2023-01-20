#include <iostream>
#include <string>

using namespace std;

// 必须返回对象时，别妄想返回其引用

// 程序员一旦学习了条款20，眼中就容不下pass by value了。
// 那样就会犯错：开始传递一些其实并不存在的对象的引用。

class Rational1 {
 public:
  Rational1(int numerator = 0, int denominator = 1)
      : n(numerator), d(denominator) {}
  void display() { cout << n << "," << d << endl; }

 private:
  int n, d;
  friend const Rational1 operator*(const Rational1& lhs, const Rational1& rhs);
};

// by value方式返回，有构造和析构的成本
const Rational1 operator*(const Rational1& lhs, const Rational1& rhs) {
  Rational1 result(lhs.n * rhs.n, lhs.d * rhs.d);
  return result;
}

// 改为传递引用，就不需要付出代价
class Rational2 {
 public:
  Rational2(int numerator = 0, int denominator = 1)
      : n(numerator), d(denominator) {}
  void display() { cout << n << "," << d << endl; }

 private:
  int n, d;
  friend const Rational2& operator*(const Rational2& lhs, const Rational2& rhs);
};

// 函数创建新对象的途径有两个：在stack或heap上创建。
#if 0
const Rational2& operator*(const Rational2& lhs, const Rational2& rhs) {
  // 在stack上创建新对象，返回局部变量的引用，崩溃：
  Rational2 result(lhs.n * rhs.n, lhs.d * rhs.d);  
  return result;
}
#else
const Rational2& operator*(const Rational2& lhs, const Rational2& rhs) {
  // 在heap内构造一个对象，必须付出一个构造函数调用的代价：
  Rational2* result = new Rational2(lhs.n * rhs.n, lhs.d * rhs.d);  // 谁删除？
  return *result;
}
#endif

class Rational3 {
 public:
  Rational3(int numerator = 0, int denominator = 1)
      : n(numerator), d(denominator) {}
  // const Rational3& operator=(const Rational3& rhs) {
  //   return Rational3(rhs.d * this->d, rhs.n * this->n);
  // }
  void display() { cout << n << "," << d << endl; }

 private:
  int n, d;
  friend const Rational3& operator*(const Rational3& lhs, const Rational3& rhs);
};
// 还有一种方法可以避免任何构造函数被调用：
const Rational3& operator*(const Rational3& lhs, const Rational3& rhs) {
  // 就像所有用上static对象的设计一样，立刻会对多线程安全性产生疑虑：
  static Rational3 result;
  // result = lhs
  return result;
}
bool operator==(const Rational3& lhs, const Rational3& rhs) { return false; }
// Rational3 w1, x1, y1, z1;
// if ((w1 * x1) == (y1 * z1)) {  // 总是true
//   cout << "xiang deng" << endl;
// } else {
//   cout << "bu xiang deng" << endl;
// }
// 在==被调用前，已经有两个*被调用，每个都返回内部定义的static对象。
// 因此==比较的都是内部的static对象，所以永远都会是true。

// 一个必须返回新对象的函数的正确写法是：就让那个函数返回一个新对象。
// 当然，需要承受返回值的构造和析构成本。
// inline const Rational1 operator*(const Rational1& lhs,
//                                  const Rational1& rhs) {
//   return Rational1(lhs.n * rhs.n, lhs.d * rhs.d);
// }
// 当必须在返回一个引用和返回一个对象之间抉择时，
// 选择返回对象，让编译器厂商尽量优化吧。

// 请记住：
// 1.绝不要返回一个指向局部对象（local stack）的指针或引用，
//   或返回指向heap对象，或返回指向局部静态对象的指针或引用。
// 2.条款4已经为在单线程环境中合理返回局部静态对象的引用提供了方法。

int main() {
  Rational1 a1(1, 2);
  Rational1 b1(3, 5);
  Rational1 c1 = a1 * b1;
  c1.display();  // 3,10

  Rational2 a2(1, 2);
  Rational2 b2(3, 5);
  Rational2 c2 = a2 * b2;
  c2.display();  // 3,10

  Rational2 w, x, y, z;
  w = x * y * z;  // 调用了两次*，使用两次new，需要两次delete
  // 无法调用delete，绝对导致内存泄露。

  return 0;
}