#include <iostream>

using namespace std;

// 不要轻易忽略编译器的警告

// 有人认为，如果问题很严重，编译器应该给一个错误信息而非警告信息。
class B {
  virtual void f() const;
};
class D : public B {
 public:
  virtual void f();
};
// 没有警告

// 警告信息天生和编译器相依，不同的编译器有不同的警告标准。
// 草率地编程然后依赖编译器为你指出错误，并不可取。
// 例如上面的函数遮掩问题，另一个编译器可能连半句抱怨和抗议都么有。

// 请记住：
// 严肃对待编译器发出的警告信息。
// 努力在你的编译器的最高（最苛刻）警告级别下争取“无任何警告”的荣誉。
// 不要多度依赖编译器的报警能力，因为不同的编译器对待事情的态度并不相同。
// 一旦移植到另一个编译器上，你原本依赖的警告信息有可能消失。

int main() { return 0; }