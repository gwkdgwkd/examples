#include <iostream>

using namespace std;

// 协助完成返回值优化

// 一个返回对象的函数很难有较高的效率，因为传值返回会导致调用对象内的构造和析构函数，这种调用是不能避免的。
// 问题很简单：一个函数要么为了保证正确的行为而返回对象要么就不这么做。
// 如果它返回了对象，就没有办法摆脱被返回的对象。

class Rational {
 public:
  Rational(int numerator = 0, int denominator = 1) {}
  friend const Rational operator*(const Rational& lhs, const Rational& rhs);
  int numerator() const;
  int denominator() const;
};
const Rational operator*(const Rational& lhs, const Rational& rhs) {}
// operator*如何能避免建立新对象来容纳它们的计算结果呢？
// 这是不可能的，所以它必须得建立新对象并返回它。
// 有时人们会返回指针，从而导致这种滑稽的句法：
class Rational1 {
 public:
  Rational1(int numerator = 0, int denominator = 1) {}
  friend const Rational1* operator*(const Rational1& lhs, const Rational1& rhs);
  int numerator() const;
  int denominator() const;
};
const Rational1* operator*(const Rational1& lhs, const Rational1& rhs) {}
// 调用者应该删除函数返回对象的指针么？答案通常是肯定的，并且通常会导致资源泄漏。
// 其它一些开发人员会返回引用。这种方法能产生可接受的句法:
class Rational2 {
 public:
  Rational2(int numerator = 0, int denominator = 1) {}
  friend const Rational2& operator*(const Rational2& lhs, const Rational2& rhs);
  int numerator() const {}
  int denominator() const {}
};
const Rational2& operator*(const Rational2& lhs, const Rational2& rhs) {
  Rational2 result(lhs.numerator() * rhs.numerator(),
                   lhs.denominator() * rhs.denominator());
  return result;  // 返回时，其指向的对象已经不存在了
}
// 这个函数返回的引用，其指向的对象已经不存在了。
// 它返回的是一个指向局部对象result的引用，当operator*退出时result被自动释放。
// 返回指向已被释放的对象的引用，这样的引用绝对不能使用。

// 一些函数（operator*也在其中）必须要返回对象。
// 这就是它们的运行方法，不要与其对抗，你不会赢的。
// 你消除传值返回的对象的努力不会获得胜利，这是一场错误的战争。
// 从效率的观点来看，你不应该关心函数返回的对象，你仅仅应该关心对象的开销。
// 你所应该关心的是把你的努力引导到寻找减少返回对象的开销上来，而不是去消除对象本身。
// 如果没有与这些对象相关的开销，谁还会关心有多少对象被建立呢？
// 以某种方法返回对象，能让编译器消除临时对象的开销，这样编写函数通常是很普遍的。
// 这种技巧是返回constructor argument而不是直接返回对象，你可以这样做：
class Rational3 {
 public:
  Rational3(int numerator = 0, int denominator = 1) {}
  friend const Rational3 operator*(const Rational3& lhs, const Rational3& rhs);
  int numerator() const {}
  int denominator() const {}
};
// 一种高效和正确的方法，用来实现返回对象的函数
inline const Rational3 operator*(const Rational3& lhs, const Rational3& rhs) {
  // 它看上去好象正在调用Rational的构造函数，实际上确是这样。
  // 你通过这个表达式建立一个临时的Rational对象，并且这是一个临时对象，函数把它拷贝给函数的返回值。
  return Rational3(lhs.numerator() * rhs.numerator(),
                   lhs.denominator() * rhs.denominator());
}
// 返回constructor argument而不出现局部对象，这种方法还会给你带来很多开销，
// 因为你仍旧必须为在函数内临时对象的构造和释放而付出代价，你仍旧必须为函数返回对象的构造和释放而付出代价。
// 但是你已经获得了好处，C++规则允许编译器优化不出现的临时对象（temporary objects out of existence）。
// 编译器就会被允许消除在operator*内的临时变量和operator*返回的临时变量。
// 它们能在为目标c分配的内存里构造return表达式定义的对象。
// 如果你的编译器这样去做，调用operator*的临时对象的开销就是零：没有建立临时对象。
// 你的代价就是调用一个构造函数――建立c时调用的构造函数。
// 而且你不能比这做得更好了，因为c是命名对象，命名对象不能被消除。
// 不过你还可以通过把函数声明为inline来消除operator*的调用开销。
// 通过使用函数的return位置（或者在函数被调用位置用一个对象来替代）来消除局部临时对象――是众所周知的和被普遍实现的。
// 它甚至还有一个名字：返回值优化（return value optimization）。
// 但注意，这种优化对普通的赋值运算无效，编译器不能够用拷贝构造函数取代赋值运算动作，
// 最终结论是：在确保语意正确的前题下没有更好的优化可能了

int main() {
  Rational1 a = 10;
  Rational1 b(1, 2);
  Rational1 c = *(a * b);

  Rational2 a1 = 10;
  Rational2 b1(1, 2);
  Rational2 c1 = a1 * b1;

  Rational3 a3 = 10;
  Rational3 b3(1, 2);
  Rational3 c3 = a3 * b3;

  return 0;
}