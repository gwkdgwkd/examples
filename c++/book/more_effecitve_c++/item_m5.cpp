#include <iostream>
#include <string>

// 谨慎定义类型转换函数

// C++编译器能够在两种数据类型之间进行隐式转换，它继承了C语言的转换方法，
// 例如允许把char隐式转换为int和从short隐式转换为double。
// 因此当把short值传递给使用double参数的函数时，依然可以成功运行。
// C中许多这种可怕的转换可能会导致数据的丢失，它们在C++中依然存在，
// 包括int到short的转换和double到char的转换。

// 对这些类型转换是无能为力的，因为它们是语言本身的特性。
// 不过当增加自己的类型时，就可以有更多的控制力，
// 因为能选择是否提供函数让编译器进行隐式类型转换。
// 有两种函数允许编译器进行这些的转换：
// 1.单参数构造函数（single-argument constructors）；
// 2.隐式类型转换运算符。
// 当在不需要使用转换函数时，这些的函数却会被调用运行。
// 结果，这些不正确的程序会做出一些令人恼火的事情，而你又很难判断出原因。

namespace n1 {
namespace test1 {
// 单参数构造函数是指只用一个参数即可以调用的构造函数。
// 该函数可以是只定义了一个参数，
// 也可以是虽定义了多个参数但第一个参数以后的所有参数都有缺省值。
class Rational {
 public:
  // 转换int到有理数类：
  Rational(int numerator = 0, int denominator = 1) {}
};
void func() {
  Rational r(1, 2);
  double d = 0.5;

  // 打印操作将失败，因为没有合适的的operator<<被调用：
  // std::cout << r;
}
}  // namespace test1

namespace test2 {
// 隐式类型转换运算符只是一个样子奇怪的成员函数：
// operator关键字，其后跟一个类型符号。
// 不用定义函数的返回类型，因为返回类型就是这个函数的名字。
// 例如为了允许Rational类隐式地转换为double类型，
// 用有理数进行混合类型运算时可能有用，可以如此声明Rational类：
class Rational {
 public:
  // 转换int到有理数类：
  Rational(int numerator = 0, int denominator = 1) {}
  // 转换Rational类成double类型：
  operator double() const {}
};

void func() {
  Rational r(1, 2);
  // 当编译器调用operator<<时，会发现没有这样的函数存在，
  // 但是它会试图找到一个合适的隐式类型转换顺序以使得函数调用正常运行。
  // 类型转换顺序的规则定义是复杂的，但是在现在这种情况下，
  // 编译器发现能调用Rational::operator double函数，把r转为double。
  std::cout << r << std::endl;  // 0.5
  // 打印的结果是一个浮点数，而不是一个有理数，隐式类型转换的缺点。
}
}  // namespace test2

namespace test3 {
// 解决方法是用不使用语法关键字的等同的函数来替代转换运算符。
// 例如为了把Rational对象转换为double，
// 用asDouble函数代替operator double函数：
class Rational {
 public:
  // 转换int到有理数类：
  Rational(int numerator = 0, int denominator = 1) {}
  double asDouble() const {}
};
// 在多数情况下，这种显式转换函数的使用虽然不方便，
// 但是函数被悄悄调用的情况不再会发生，这点损失是值得的。
// 一般来说，越有经验的C++程序员就越喜欢避开类型转换运算符。
// 库函数中的string类型没有包括隐式地从string转换成C风格的char*的功能，
// 而是定义了一个成员函数c_str用来完成这个转换。

void func() {
  Rational r(1, 2);
  // std::cout << r;  // 错误! Rational对象没有operator<<
  std::cout << r.asDouble() << std::endl;  // 4.68391e-310
}
}  // namespace test3

void func() {
  test1::func();
  test2::func();
  test3::func();
}
}  // namespace n1

namespace n2 {
// 通过单参数构造函数进行隐式类型转换更难消除。
// 而且在很多情况下这些函数所导致的问题要甚于隐式类型转换运算符。

namespace test1 {
template <class T>
class Array {
 public:
  // 不能做为类型转换函数：
  Array(int lowBound, int highBound);
  // 能做为类型转换函数使用，能导致无穷的痛苦：
  Array(int size) {}
  T& operator[](int index) {}
  friend bool operator==(const Array<int>& lhs, const Array<int>& rhs);
};
bool operator==(const Array<int>& lhs, const Array<int>& rhs) {}
void func() {
  Array<int> a(10);
  Array<int> b(10);
  for (int i = 0; i < 10; ++i) {
    // 希望编译器能报出各种各样的警告信息，但是它根本没有，编译器当成下面这样：
    // if(a == static_cast< Array<int> >(b[i]))
    if (a == b[i]) {  // 编辑错误，a应该是a[i]
      std::cout << "a[" << i << "] == b[" << i << "]" << std::endl;
    } else {
      std::cout << "a[" << i << "] != b[" << i << "]" << std::endl;
    }
  }
}
}  // namespace test1

// 想提供一个单参数构造函数，同时也希望防止编译器不加鉴别地调用这个构造函数。
// 有两个方法：
namespace test2 {
// 1.容易的方法，利用一个最新编译器的特性（为了解决隐式类型转换），explicit关键字。
//   构造函数用explicit，编译器会拒绝为了隐式转换而调用构造函数，显式转换依然合法。
template <class T>
class Array {
 public:
  explicit Array(int size) {}
  T& operator[](int index) {}
  friend bool operator==(const Array<int>& lhs, const Array<int>& rhs);
};
bool operator==(const Array<int>& lhs, const Array<int>& rhs) {}
void func() {
  // 正确, explicit构造函数在建立对象时能正常使用：
  Array<int> a(10);
  Array<int> b(10);  // 也正确
  for (int i = 0; i < 10; ++i) {
    // 错误，没有办法隐式转换int到Array<int>：
    // if (a == b[i]) {}
    if (a == Array<int>(b[i])) {
      // 正确,显式从int到Array<int>转换，但是代码的逻辑不合理
    }
    if (a == static_cast<Array<int> >(b[i])) {
      // 同样正确，同样不合理
    }
    if (a == (Array<int>)b[i]) {
      // C风格的转换也正确，但是逻辑依旧不合理
    }
  }
}
}  // namespace test2

namespace test3 {
// 2.如果编译器不支持explicit，必须修改单参数构造函数的参数，用ArraySize代替int。
template <class T>
class Array {
 public:
  // 这里把ArraySize嵌套入Array中，为了强调它总是与Array一起使用。
  // 也必须声明ArraySize为公有，为了让任何人都能使用它。
  class ArraySize {  // 这个类是新的
   public:
    ArraySize(int numElements) : theSize(numElements) {}
    int size() const { return theSize; }

   private:
    int theSize;
  };
  Array(int lowBound, int highBound) {}
  Array(ArraySize size) {}
  T& operator[](int index) {}
  friend bool operator==(const Array<int>& lhs, const Array<int>& rhs);
};
bool operator==(const Array<int>& lhs, const Array<int>& rhs) {}
// 类似于ArraySize的类经常被称为proxy class，M30专门讨论proxy class。
void func() {
  // 编译器要求用int参数调用Array<int>里的构造函数，但是没有这样的构造函数。
  // 编译器意识到它能从int参数转换成一个临时ArraySize对象，
  // ArraySize对象只是Array<int>构造函数所需要的，这样编译器进行了转换。
  Array<int> a(10);
  // 事实上仍旧能够安心地构造Array对象，不过这样做能够使你避免类型转换：
  for (int i = 0; i < 10; ++i) {
    // 为了调用operator==函数，编译器要求Array<int>对象在==右侧，
    // 但是不存在一个参数为int的单参数构造函数。
    // 而且编译器无法把int转换成一个临时ArraySize对象，
    // 然后通过这个临时对象建立必须的Array<int>对象，
    // 因为这将调用两个用户定义（user-defined）的类型转换，
    // 一个从int到ArraySize，一个从ArraySize到Array<int>。
    // 这种转换顺序被禁止的，所以当试图进行比较时编译器肯定会产生错误。
    // if (a2 == b2[i]) {}  // 现在是一个错误
  }
}
}  // namespace test3

// 让编译器进行隐式类型转换所造成的弊端要大于它所带来的好处，
// 所以除非确实需要，不要定义类型转换函数。
}  // namespace n2

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
      n2::test1::func();
      break;
    case 2:
      n2::test2::func();
      break;
    case 3:
      n2::test3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}