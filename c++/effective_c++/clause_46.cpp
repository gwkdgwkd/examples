#include <iostream>

using namespace std;

// 需要类型转换时请为模板定义非成员函数

// 条款24讨论过为什么：
// 惟有non-member函数才有能力在所有实参身上实施隐式类型转换。
template <typename T>
class Rational {
 public:
  Rational(const T& numerator = 0, const T& denominator = 1) {}
  const T numerator() const;
  const T denominator() const;
};
template <typename T>
const Rational<T> operator*(const Rational<T>& lhs, const Rational<T>& rhs) {
  return lhs;
}

// template class内的friend声明可以指定某个特定函数。
// 意味着class Rational<T>可以声明为operator*是它的一个friend函数。
// class template并不依赖template实参推导，
// 所以编译器总是能够在class Rational<T>具现化时得知T。
template <typename T>
class Rational1 {
 public:
  Rational1(const T& numerator = 0, const T& denominator = 1) {}
  const T numerator() const;
  const T denominator() const;
  // template名称可以被用来作为template和其参数的间略表达式：
  friend const Rational1 operator*(const Rational1& lhs, const Rational1& rh);
  // friend const Rational1<T> operator*(const Rational1<T>& lhs,
  //                                     const Rational1<T>& rh); // 也可以
};
// 只是在template声明，没有被定义出来。
// 让class外部的operate* template提供定义，行不通。
// 自己声明了一个函数，就有责任定义那个函数。
// 既然没有提供定义，连接器当然找不到它。
template <typename T>
const Rational1<T> operator*(const Rational1<T>& lhs, const Rational1<T>& rhs) {
  return lhs;
}

// 最简单可行的办法就是将operator*的函数本体合并至声明内：
template <typename T>
class Rational2;
template <typename T>
const Rational2<T> doMultiply(const Rational2<T>& lhs, const Rational2<T>& rh);
template <typename T>
class Rational2 {
 public:
  Rational2(const T& numerator = 0, const T& denominator = 1) {}
  const T numerator() const { return n; };
  const T denominator() const { return d; };
  // 虽然使用friend，却与friend的传统用途毫不相干。
  // 为了令这个函数自动具现化，需要将它声明在class内部；
  // 而在class内部声明non-member函数的唯一办法就是，令它成为一个friend。
  friend const Rational2 operator*(const Rational2& lhs, const Rational2& rhs) {
    // 自动成为inline，要减小inline带来的冲击最小化：
    // 可以令operator*不做任何事情，只调用一个定义于class外部的辅助函数。
    return doMultiply(lhs, rhs);
  }

 private:
  int n;
  int d;
};
template <typename T>
const Rational2<T> doMultiply(const Rational2<T>& lhs,
                              const Rational2<T>& rhs) {
  return Rational2<T>(lhs.numerator() * rhs.numerator(),
                      lhs.denominator() * rhs.denominator());
}

// 请记住：
// 当我们编写一个class template时，
// 它提供的与此template相关的函数支持所有参数的隐式类型转换时，
// 请将那些函数定义为class template内部的friend函数。

int main() {
  // 条款24的例子，只不过换成来template：
  Rational<int> oneHalf(1, 2);
  // Rational<int> result = oneHalf * 2;  // 错误
  // 编译器知道尝试调用什么函数，
  // 但不知道想调用哪个函数，推导不出来。
  // 编译器在实参推到过程中从不将隐式转换函数考虑在内。

  Rational1<int> oneHalf1(1, 2);
  // 可以编译通过，但是链接失败：
  // Rational1<int> result1 = oneHalf1 * 2;  /

  Rational2<int> oneHalf2(1, 2);
  Rational2<int> result2 = oneHalf2 * 2;

  return 0;
}