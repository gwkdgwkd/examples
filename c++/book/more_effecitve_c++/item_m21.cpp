#include <iostream>

// 通过重载避免隐式类型转换

namespace n1 {
class UPInt {
 public:
  UPInt() {}
  UPInt(int value) {}
};
const UPInt operator+(const UPInt& lhs, const UPInt& rhs) {}

void func() {
  UPInt upi1, upi2;
  UPInt upi3 = upi1 + upi2;

  // 方法是通过建立临时对象把整形数10转换为UPInts，编译器完成这种类型转换很方便，
  // 但是建立临时对象进行类型转换工作是有开销的，而不想承担这种开销。
  upi3 = upi1 + 10;
  upi3 = 10 + upi2;
  // 认识到不是真的要进行类型转换，而是用UPint和int做为参数调用operator+。
  // 隐式类型转换只是用来达到目的的手段，但不要混淆手段与目的。
}
}  // namespace n1

namespace n2 {
// 还有一种方法可以进行operator+的混合类型调用，它将消除隐式类型转换的需要。
// 如果想要把UPInt和int对象相加，通过声明如下几个函数达到这个目的：
class UPInt {
 public:
  UPInt() {}
  explicit UPInt(int value) {}
};
const UPInt operator+(const UPInt& lhs, const UPInt& rhs) {}
const UPInt operator+(const UPInt& lhs, int rhs) {}
const UPInt operator+(int lhs, const UPInt& rhs) {}
// 一旦开始用函数重载来消除类型转换，就有可能这样声明函数，把自己陷入危险之中：
// const UPInt operator+(int lhs, int rhs);  // 错误!
// 在C++中有一条规则是：
// 每一个重载的operator必须带有一个用户定义类型的参数。
// 如果没有这条规则，程序员将能改变预定义的操作，这样就把程序引入混乱的境地。

void func() {
  UPInt upi1, upi2;
  UPInt upi3 = upi1 + upi2;  // 正确，没有临时对象
  upi3 = upi1 + 10;          // 正确，没有临时对象
  upi3 = 10 + upi2;          // 正确，没有临时对象
}

// 利用重载避免临时对象的方法不只是用在operator函数上。
// 比如在大多数程序中，想允许在所有能使用string对象的地方，
// 也一样可以使用char*，反之亦然。
// 同样如果正在使用numerical（数字）类，例如complex，
// 让int和double这样的类型可以使用在numerical对象的任何地方。
// 因此任何带有string、char*、complex的函数，可以采用重载来消除类型转换。

// 不过，必须谨记80-20规则，没有必要实现大量的重载函数，
// 除非有理由确信程序使用重载函数以后其整体效率会有显著的提高。
}  // namespace n2

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}