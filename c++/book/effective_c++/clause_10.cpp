#include <iostream>

// 令operator=返回一个reference to *this

// 为了实现连锁赋值，赋值操作符必须返回一个reference指向操作符的左侧实参。
class Widget {
 public:
  // 标准赋值形式，也适用于所有赋值相关运算：
  Widget &operator=(const Widget &rhs) { return *this; }
};
// 上面只是个协议，并无强制性，不遵守也能编译通过，
// 但是string，vector，complex，tr1::shared_ptr都准守这个协议。

// 请记住：
// 1.令赋值操作符号返回一个reference to *this。

int main() {
  Widget a, b, c;
  a = b = c;

  return 0;
}