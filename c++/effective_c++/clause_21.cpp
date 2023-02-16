#include <iostream>
#include <string>

// 必须返回对象时，别妄想返回其引用

// 程序员一旦学习了条款20，眼中就容不下pass by value了，
// 那样就会犯错：开始传递一些其实并不存在的对象的引用。

namespace n1 {
class Rational {
 public:
  Rational(int numerator = 0, int denominator = 1)
      : n(numerator), d(denominator) {}
  void display() { std::cout << n << "," << d << std::endl; }

 private:
  int n, d;
  friend const Rational operator*(const Rational& lhs, const Rational& rhs);
};

// by value方式返回，有构造和析构的成本：
const Rational operator*(const Rational& lhs, const Rational& rhs) {
  Rational result(lhs.n * rhs.n, lhs.d * rhs.d);
  return result;
}
void func() {
  Rational a(1, 2);
  Rational b(3, 5);
  Rational c = a * b;
  c.display();  // 3,10
}
}  // namespace n1

namespace n2 {
// 改为传递引用，就不需要付出代价
namespace test1 {
class Rational {
 public:
  Rational(int numerator = 0, int denominator = 1)
      : n(numerator), d(denominator) {}
  void display() { std::cout << n << "," << d << std::endl; }

 private:
  int n, d;
  friend const Rational& operator*(const Rational& lhs, const Rational& rhs);
};
const Rational& operator*(const Rational& lhs, const Rational& rhs) {
  // 在stack上创建新对象，返回局部变量的引用，崩溃：
  Rational result(lhs.n * rhs.n, lhs.d * rhs.d);
  return result;
}
void func() {
  Rational a(1, 2);
  Rational b(3, 5);
  Rational c = a * b;
  c.display();  // 3,10
}
}  // namespace test1
namespace test2 {
class Rational {
 public:
  Rational(int numerator = 0, int denominator = 1)
      : n(numerator), d(denominator) {}
  void display() { std::cout << n << "," << d << std::endl; }

 private:
  int n, d;
  friend const Rational& operator*(const Rational& lhs, const Rational& rhs);
};
const Rational& operator*(const Rational& lhs, const Rational& rhs) {
  // 在heap内构造一个对象，必须付出一个构造函数调用的代价：
  Rational* result = new Rational(lhs.n * rhs.n, lhs.d * rhs.d);  // 谁删除？
  return *result;
}
void func() {
  Rational a(1, 2);
  Rational b(3, 5);
  Rational c = a * b;
  c.display();  // 3,10

  Rational w, x, y, z;
  w = x * y * z;
  // 调用了两次*，使用两次new，需要两次delete，
  // 无法调用delete，绝对导致内存泄露。
}
}  // namespace test2
void func() {
  test1::func();
  test2::func();
}
}  // namespace n2

namespace n3 {
class Rational {
 public:
  Rational(int numerator = 0, int denominator = 1)
      : n(numerator), d(denominator) {}
  void display() { std::cout << n << "," << d << std::endl; }

 private:
  int n, d;
  friend const Rational& operator*(const Rational& lhs, const Rational& rhs);
  friend bool operator==(const Rational& lhs, const Rational& rhs);
};
// 还有一种方法可以避免任何构造函数被调用：
const Rational& operator*(const Rational& lhs, const Rational& rhs) {
  // 就像所有用上static对象的设计一样，立刻会对多线程安全性产生疑虑：
  static Rational result;
  return result;
}
bool operator==(const Rational& lhs, const Rational& rhs) {
  return lhs.n == rhs.n && lhs.d == rhs.d;
}

void func() {
  Rational a(1, 2);
  Rational b(3, 4);
  Rational c(5, 6);
  Rational d(7, 8);
  if ((a * b) == (c * d)) {  // 总是true
    std::cout << "xiang deng" << std::endl;
  } else {
    std::cout << "bu xiang deng" << std::endl;
  }
  // xiang deng

  // 在==被调用前，已经有两个*被调用，每个都返回内部定义的static对象，
  // 因此==比较的都是内部的static对象，所以永远都会是true。
}
}  // namespace n3

namespace n4 {
// 一个必须返回新对象的函数的正确写法是：就让那个函数返回一个新对象。
// 当然，需要承受返回值的构造和析构成本。
// 当必须在返回引用和返回对象间抉择时，选择返回对象，让编译器厂商尽量优化吧。
class Rational {
 public:
  Rational(int numerator = 0, int denominator = 1)
      : n(numerator), d(denominator) {}
  void display() { std::cout << n << "," << d << std::endl; }

 private:
  int n, d;
  friend const Rational operator*(const Rational& lhs, const Rational& rhs);
};
const Rational operator*(const Rational& lhs, const Rational& rhs) {
  return Rational(lhs.n * rhs.n, lhs.d * rhs.d);
}
void func() {
  Rational a(1, 2);
  Rational b(3, 5);
  Rational c = a * b;
  c.display();  // 3,10

  Rational w, x, y, z;
  w = x * y * z;
}
}  // namespace n4

// 请记住：
// 1.绝不要返回一个指向局部对象（local stack）的指针或引用，
//   或返回指向heap对象，或返回指向局部静态对象的指针或引用；
// 2.条款4已经为在单线程环境中合理返回局部静态对象的引用提供了方法。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::func();
      break;
    case 2:
      n3::func();
      break;
    case 3:
      n4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}