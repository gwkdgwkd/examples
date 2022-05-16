#include <iostream>

using namespace std;

// 通过重载避免隐式类型转换

class UPInt {  // unlimited precision integers类
 public:
  UPInt() {}
  UPInt(int value) {}
};
const UPInt operator+(const UPInt& lhs, const UPInt& rhs) {}

// 还有一种方法可以成功进行operator+的混合类型调用，它将消除隐式类型转换的需要。
// 如果我们想要把UPInt和int对象相加，通过声明如下几个函数达到这个目的，每一个函数有不同的参数类型集。
class UPInt1 {  // unlimited precision integers类
 public:
  UPInt1() {}
  UPInt1(int value) {}
};
const UPInt1 operator+(const UPInt1& lhs, const UPInt1& rhs) {}
const UPInt1 operator+(const UPInt1& lhs, int rhs) {}
const UPInt1 operator+(int lhs, const UPInt1& rhs) {}
// 一旦你开始用函数重载来消除类型转换，你就有可能这样声明函数，把自己陷入危险之中：
// const UPInt operator+(int lhs, int rhs);  // 错误!
// 在C++中有一条规则是每一个重载的operator必须带有一个用户定义类型（user-defined type）的参数。
// 如果没有这条规则，程序员将能改变预定义的操作，这样做肯定把程序引入混乱的境地。

// 利用重载避免临时对象的方法不只是用在operator函数上。
// 比如在大多数程序中，你想允许在所有能使用string对象的地方，也一样可以使用char*，反之亦然。
// 同样如果你正在使用numerical（数字）类，
// 例如complex,你想让int和double这样的类型可以使用在numerical对象的任何地方。
// 因此任何带有string、char*、complex参数的函数可以采用重载方式来消除类型转换。

// 不过，必须谨记80－20规则。
// 没有必要实现大量的重载函数，除非你有理由确信程序使用重载函数以后其整体效率会有显著的提高。

int main() {
  UPInt upi1, upi2;
  UPInt upi3 = upi1 + upi2;

  // 方法是通过建立临时对象把整形数10转换为UPInts
  // 让编译器完成这种类型转换是确实是很方便，但是建立临时对象进行类型转换工作是有开销的，
  // 而我们不想承担这种开销。
  upi3 = upi1 + 10;
  upi3 = 10 + upi2;
  // 认识到我们的目的不是真的要进行类型转换，而是用UPint和int做为参数调用operator+。
  // 隐式类型转换只是用来达到目的的手段，但是我们不要混淆手段与目的。

  UPInt1 upi11, upi12;
  UPInt1 upi13 = upi11 + upi12;  // 正确，没有由upi1或upi2生成的临时对象
  upi13 = upi11 + 10;  // 正确，没有由upi1 or 10生成的临时对象
  upi13 = 10 + upi12;  //正确，没有由10 or upi2生成的临时对象

  return 0;
}