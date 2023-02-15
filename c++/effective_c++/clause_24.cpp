#include <iostream>

// 若所有参数皆需类型转换，请为此采用non-member函数

namespace n1 {
// 令class支持隐式类型转换通常是个糟糕的主意，最常见的例外是在建立数值类型时：
class Rational {
 public:
  // 没有explicit，允许int to Rational1隐式转换：
  Rational(int numerator = 0, int denominator = 1)
      : n(numerator), d(denominator) {}
  int numerator() const { return n; };
  int denominator() const { return d; };
  void display() { std::cout << n << "," << d << std::endl; }

  // 不确定用member、non-member或non-member friend来实现它们：
  const Rational operator*(const Rational& rhs) const {  // member函数
    Rational result(this->numerator() * rhs.numerator(),
                    this->denominator() * rhs.denominator());
    return result;
  }

 private:
  int n, d;
};
void func() {
  // Rational可以使两个有理数以最轻松的方式相乘：
  Rational a(2, 8);
  Rational b(2, 2);
  Rational c = a * b;
  c.display();  // 4,16

  // 尝试混合运算，乘法应该满足交换率：
  c = a * 2;
  // 转换为：c = a.operator*(2);
  // 发生隐式类型转换，编译知道正在传递int，但函数需要Rational，
  // 也知道调用构造函数就能把int变成Rational，于是就那样做了。
  // 当然，只有non-explicit构造函数，编译器才会这么做。
  c.display();  // 4,8

  // c = 2 * a;  // 错误
  // 转换为：c = 2.operator*(a);
  // 只有当参数被列于参数列内，这个参数才是隐式类型转换的合格参与者，
  // 2没有operator*，所以c = 2 * a不能通过编译，this指针不是合格的参与者。
}
}  // namespace n1

namespace n2 {
class Rational {
 public:
  explicit Rational(int numerator = 0, int denominator = 1)
      : n(numerator), d(denominator) {}
  int numerator() const { return n; };
  int denominator() const { return d; };

  const Rational operator*(const Rational& rhs) const {
    Rational result(this->numerator() * rhs.numerator(),
                    this->denominator() * rhs.denominator());
    return result;
  }

 private:
  int n, d;
};

void func() {
  Rational a(2, 8);
  Rational c;
  // c = a * 2;  // explicit构造函数，禁止来隐式类型转换
}
}  // namespace n2

namespace n3 {
// 让operator*成为non-member函数，编译器允许每个实参身上执行隐式类型转换：
class Rational {
 public:
  Rational(int numerator = 0, int denominator = 1)
      : n(numerator), d(denominator) {}
  int numerator() const { return n; };
  int denominator() const { return d; };
  void display() { std::cout << n << "," << d << std::endl; }

 private:
  int n, d;
};
const Rational operator*(const Rational& lhs, const Rational& rhs) {
  return Rational(lhs.numerator() * rhs.numerator(),
                  lhs.denominator() * rhs.denominator());
}

// Rational的operator*是否应该成为friend函数呢？
// 对Rational3来说，答案式否定的，
// 因为operator*完全可以通过Rational的public接口完成任务。
// 无论何时如果能避免friend函数就该避免，朋友带来的麻烦往往多个其价值。
// 不应该只因为函数不该成为member，就自动让它成为friend。
void func() {
  Rational a(2, 8);
  Rational b(2, 2);
  Rational c = a * b;
  c.display();  // 4,16

  c = a * 2;  // 4,8
  c.display();

  c = 2 * a;    // non-member可以通过编译
  c.display();  // 4,8
}
}  // namespace n3

// 请记住：
// 1.如果需要为某个函数的所有参数，包括被this所指的哪个隐喻参数进行类型转换，
//   那么这个函数必须是个non-member。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}