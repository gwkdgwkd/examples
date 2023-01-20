#include <iostream>
using namespace std;

// 若所有参数皆需类型转换，请为此采用non-member函数

// 令class支持隐式类型转换通常是个糟糕的主意，最常见的例外是在建立数值类型时。
class Rational1 {
 public:
  // 没有explicit，允许int to Rational1隐式转换：
  Rational1(int numerator = 0, int denominator = 1)
      : n(numerator), d(denominator) {}
  int numerator() const { return n; };
  int denominator() const { return d; };
  void display() { cout << n << "," << d << endl; }

  // 不确定用member、non-member或non-member friend来实现它们：
  const Rational1 operator*(const Rational1& rhs) const {  // member函数
    Rational1 result(this->numerator() * rhs.numerator(),
                     this->denominator() * rhs.denominator());
    return result;
  }

 private:
  int n, d;
};

class Rational2 {
 public:
  explicit Rational2(int numerator = 0, int denominator = 1)
      : n(numerator), d(denominator) {}
  int numerator() const { return n; };
  int denominator() const { return d; };

  const Rational1 operator*(const Rational1& rhs) const {
    Rational1 result(this->numerator() * rhs.numerator(),
                     this->denominator() * rhs.denominator());
    return result;
  }

 private:
  int n, d;
};

// 让operator*成为一个non-member函数，
// 编译器允许每个实参身上执行隐式类型转换：
class Rational3 {
 public:
  Rational3(int numerator = 0, int denominator = 1)
      : n(numerator), d(denominator) {}
  int numerator() const { return n; };
  int denominator() const { return d; };

 private:
  int n, d;
};
const Rational3 operator*(const Rational3& lhs, const Rational3& rhs) {
  return Rational3(lhs.denominator() * rhs.denominator(),
                   lhs.numerator() * rhs.numerator());
}

// Rational3的operator*是否应该成为friend函数呢？
// 对Rational3来说，答案式否定的。
// 因为operator*完全可以通过Rational3的public接口完成任务。
// 无论何时如果能避免friend函数就该避免，朋友带来的麻烦往往多个其价值。
// 不应该只因为函数不该成为member，就自动让它成为friend。

// 请记住：
// 1.如果需要为某个函数的所有参数，
//   包括被this指针所指的哪个隐喻参数进行类型转换，
//   那么这个函数必须是个non-member。

int main() {
  // Rational1可以使两个有理数以最轻松的方式相乘：
  Rational1 a1(2, 8);
  Rational1 b1(2, 2);
  Rational1 c1 = a1 * b1;
  c1 = c1 * a1;
  c1.display();  // 8,128

  // 尝试混合运算(乘法应该满足交换率)：
  c1 = a1 * 2;  // 很好，result = a1.operator*(2);

  // 错误，result = 2.operator*(al); 2没有operator*成员函数：
  // c1 = 2 * a1;

  // 发生来隐式类型转换，编译知道正在传递int，但函数需要Rational1；
  // 也知道调用构造函数就能把int变成Rational1，于是就那样做了。
  // 当然，只有non-explicit构造函数，编译器才会这么做。
  // c1 = a1 * 2;

  // 只有当参数被列于参数列内，这个参数才是隐式类型转换的合格参与者。
  // 所以c1 = 2 * a1不能通过编译。
  // this指针不是合格的参与者。

  Rational2 a2(2, 8);
  Rational2 c2;
  // c2 = a2 * 2;  // explicit构造函数，禁止来隐式类型转换

  Rational3 a3(2, 8);
  Rational3 b3(2, 2);
  Rational3 c3 = a3 * b3;
  c3 = a3 * 2;
  c3 = 2 * a3;  // non-member可以通过编译

  return 0;
}