#include <iostream>

// 若不想使用编译器自动生成的函数，就该明确拒绝

namespace n1 {
// 如果不希望class支持某一特定机能，只要不声明对应函数就是了。
// 但对copy构造函数和copy assignment操作符却不起作用。
// 如果不声明它们，而某些人尝试调用它们，编译器会声明它们。
// 编译器产生的函数都是public，为阻止这些函数被创建出来，
// 需要声明为private的，即阻止了编译生成，也阻止别人调用它。
// 这样的做法并不绝对安全，
// 因为member函数和friend函数还是可以调用private函数。
// 除非够聪明，不去定义它们，
// 那么如果某些人不慎调用任何一个，会获得一个链接错误。
// 将成员函数声明为private而且故意不实现它们这一伎俩为大家接受，
// C++ iostream库(ios_base,basic_ios和sentry)中，
// 也是通过这样的方法阻止copying行为的。
class HomeForSale {
 public:
  HomeForSale() {}

 private:
  // 参数名称并非必要，函数不会被实现，指定参数名称也没用：
  HomeForSale(const HomeForSale&);
  HomeForSale& operator=(const HomeForSale&);
};
// 如果不慎在member函数或friend函数之内那么做，链接失败。
}  // namespace n1

namespace n2 {
// 将n1的链接期错误移至编译期是可能的，而且是好事，越早发现越好。
class Uncopyable {
 protected:
  Uncopyable() {}
  ~Uncopyable() {}

 private:
  Uncopyable(const Uncopyable&);
  Uncopyable& operator=(const Uncopyable&);
};
// class不再声明copy构造函数或copy assignment操作符。
class HomeForSale : private Uncopyable {};
// 任何人（包括member函数或friend函数）尝试拷贝HomeForSale1对象，
// 编译器便尝试生成，还会调用基类对应的函数，但会被编译器拒绝，因为是私有的。
}  // namespace n2

// 请记住：
// 1.为驳回编译器自动提供的能力，可将相应的成员函数声明为private且不实现；
// 2.使用uncopyable这样的base class也是一种做法。

int main() {
  n1::HomeForSale h1, h2;
  // 当企图拷贝HomeForSale对象，编译会阻止他：
  // h1 = h2;
  // HomeForSale h3(h1);

  n2::HomeForSale h4, h5;
  // h4 = h5;
  // HomeForSale1 h6(h4);

  return 0;
}