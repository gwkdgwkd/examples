#include <iostream>

using namespace std;

// 自增(increment)、自减(decrement)操作符前缀形式与后缀形式的区别

class UPInt {
 public:
  UPInt& operator++() {  // ++前缀
    cout << "++UPInt" << endl;
    *this += 1;    // 增加
    return *this;  // 取回值
  }
  const UPInt operator++(int) {  // ++后缀
    cout << "UPInt++" << endl;
    UPInt oldValue = *this;  // 取回值
    ++(*this);               // 增加
    return oldValue;         // 返回被取回的值
  }
  UPInt& operator--() {  //--前缀
    cout << "--UPInt" << endl;
    *this -= 1;    // 增加
    return *this;  // 取回值
  }
  const UPInt operator--(int) {  // --后缀
    cout << "UPInt--" << endl;
    UPInt oldValue = *this;  // 取回值
    --(*this);               // 增加
    return oldValue;         // 返回被取回的值
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

// 前缀与后缀形式返回值类型是不同的。前缀形式返回一个引用，后缀形式返回一个const类型。
// 前缀形式有时叫做“增加然后取回”，后缀形式叫做“取回然后增加”。
// 后缀操作符函数没有使用它的参数。它的参数只是用来区分前缀与后缀函数调用。
// 如果你没有在函数里使用参数，许多编译器会显示警告信息，很令人讨厌。
// 为了避免这些警告信息，一种经常使用的方法时省略掉你不想使用的参数名称。

// 很明显一个后缀increment必须返回一个对象（它返回的是增加前的值），但是为什么是const对象呢？
// 不是const对象，下面的代码就是正确的：
class A {
 public:
  A operator++(int) {}
};
// A a;
// a++++;
// 有两个理由导致我们应该厌恶上述这种做法:
//  1 是与内置类型行为不一致。
//  2 使用两次后缀increment所产生的结果与调用者期望的不一致。
//    第二次调用operator++改变的值是第一次调用返回对象的值，而不是原始对象的值。

// 如果仅为了提高代码效率，调用者应该尽量使用前缀increment，少用后缀increment，除非确实需要使用后缀increment。
// 当处理用户定义的类型时，尽可能地使用前缀increment，因为它的效率较高。

// 如何确保后缀increment和前缀increment的行为一致呢？
// 这个原则是后缀increment和decrement应该根据它们的前缀形式来实现。
// 你仅仅需要维护前缀版本，因为后缀形式自动与前缀形式的行为一致。

int main() {
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

  A a;
  a++ ++;
  int b;
  // b++ ++;  // 报错
  // i++ ++;  // 报错

  return 0;
}