#include <iostream>

// 自增(increment)、自减(decrement)操作符前缀形式与后缀形式的区别

namespace n1 {
class UPInt {
 public:
  // 前缀与后缀形式返回值类型是不同的：
  // 1.前缀形式返回一个引用：
  UPInt& operator++() {  // ++前缀
    std::cout << "++UPInt" << std::endl;
    // 前缀形式叫做增加然后取回：
    *this += 1;    // 增加
    return *this;  // 取回值
  }
  // 2.后缀形式返回一个const类型：
  const UPInt operator++(int) {  // ++后缀
    std::cout << "UPInt++" << std::endl;
    // 后缀形式叫做取回然后增加：
    UPInt oldValue = *this;  // 取回值
    ++(*this);               // 增加
    return oldValue;         // 返回被取回的值
  }

  // 如果仅为了提高代码效率，调用者应该尽量使用前缀increment，
  // 少用后缀increment，除非确实需要使用后缀increment。
  // 当处理用户定义类型时，尽可能地使用前缀increment，因为效率较高。
  UPInt& operator--() {  //--前缀
    std::cout << "--UPInt" << std::endl;
    *this -= 1;    // 增加
    return *this;  // 取回值
  }

  // 后缀操作符函数没有使用它的参数，它的参数只是用来区分前缀与后缀函数调用。
  // 如果没有在函数里使用参数，许多编译器会显示警告信息，很令人讨厌。
  // 为了避免这些警告信息，常用的方法是只写参数类型不写参数名称。
  const UPInt operator--(int) {  // --后缀
    std::cout << "UPInt--" << std::endl;
    UPInt oldValue = *this;  // 取回值
    // 如何确保后缀increment和前缀increment的行为一致呢？
    // 这个原则是后缀increment和decrement应该根据它们的前缀形式来实现，
    // 仅仅需要维护前缀版本，因为后缀形式自动与前缀形式的行为一致：
    --(*this);        // 增加
    return oldValue;  // 返回被取回的值
  }
  UPInt& operator+=(int a) {
    i += a;
    return *this;
  }
  UPInt& operator-=(int a) {
    i -= a;
    return *this;
  }

 private:
  int i;
};
void func() {
  UPInt i;
  ++i;  // 调用i.operator++();
  i++;  // 调用i.operator++(0);
  --i;  // 调用i.operator--();
  i--;  // 调用i.operator--(0);
  // ++UPInt
  // UPInt++
  // ++UPInt
  // --UPInt
  // UPInt--
  // --UPInt
}
}  // namespace n1

namespace n2 {
// 很明显一个后缀increment必须返回一个对象，它返回的是增加前的值，
// 但一定要是const对象，否则下面的代码就是正确的：
class A {
 public:
  A& operator++() {
    (*this) += 1;
    return *this;
  }
  A operator++(int) {
    A old = *this;
    ++(*this);
    return old;
  }
  A& operator+=(int a) {
    i += a;
    return *this;
  }
  int i = 0;
};

void func() {
  A a;
  std::cout << a.i << std::endl;  // 0
  a++ ++;

  // 有两个理由应该厌恶上述这种做法:
  // 1.是与内置类型行为不一致：
  int b;
  // b++ ++;  // 报错

  // 2.使用两次后缀increment所产生的结果与调用者期望的不一致，
  //   第二次operator++改变是第一次调用返回对象的值，而不是原始对象的值：
  std::cout << a.i << std::endl;  // 1
}
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