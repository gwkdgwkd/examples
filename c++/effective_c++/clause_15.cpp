#include <iostream>
#include <memory>

using namespace std;

// 在资源管理类中提供对原始资源的访问

// 许多API需要直接操作资源，除非不使用这样的API，
// 否则需要提供直接访问原始资源的方法：
// 1.shared_ptr和auto_ptr都提供一个get成员函数，
//   用来执行显示转换，也就是返回智能指针内部的原始指针；
// 2.shared_ptr和auto_ptr也重载了指针取值操作符，
//   允许隐式转换为原始指针；
// 3.某些RAII class设计者联想到将油脂涂在滑轨上，
//   提供一个隐式转换函数。
struct FontHandle {};
FontHandle getFont() {}  // C API
void releaseFont(FontHandle fh) {}
class Font {  // RAII class
 public:
  explicit Font(FontHandle fh) : f(fh) {}
  ~Font() { releaseFont(f); }
  FontHandle get() const { return f; }  // 显示转换函数
  // 每当使用API时就要调用get，不方便。
  operator FontHandle() const { return f; }  // 隐式转换函数

 private:
  FontHandle f;  // 原始资源
};

// 是否提供一个显示转换函数将RAII class转换为其底部资源，
// 或是应该提供隐式转换，
// 答案主要取决于RAII class被设计执行的特定工作，以及被使用的情况。
// 通常显示转换函数比较受欢迎，因为它将非故意之类型转换的可能性最小化了。
// 然后有时候，隐式类型转换所带来的自然用法也会引发天秤倾斜。

// RAII class内返回原始资源的函数，与封装发生矛盾。
// 但一般而言它谈不上是什么设计灾难，
// RAII class并不是为了封装某物而存在的。

// 请记住：
// API往往要求访问原始资源，
// 所以每一个RAII class应该提供一个取得其所管理资源的方法。
// 对原始资源的访问可能经由显式转换或隐式转换。
// 一般显式转换比较安全，但隐式转换对客户比较方便。

void changeFontSize(FontHandle f, int newSize) {}
int main() {
  Font f(getFont());
  int newFontSize;
  changeFontSize(f.get(), newFontSize);
  changeFontSize(f, newFontSize);

  Font f1(getFont());
  // 隐式转换会增加错误发生的机会，
  // 需要Font时意外创建一个FontHandle：
  FontHandle f2 = f1;
  // 原意是要拷贝一个Font对象，
  // 却反而将f1隐式转换为其底部的FontHandle，然后才复制它。

  return 0;
}