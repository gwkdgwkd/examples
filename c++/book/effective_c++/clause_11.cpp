#include <iostream>

// 在operator=中处理自我赋值

class Bitmap {};
namespace n1 {
class Widget {
 public:
  Widget() : pb(new Bitmap) {}
  // 下面的代码表面看起来合理，但自我赋值出现时并不安全：
  Widget& operator=(const Widget& rhs) {
    // 删除当前的bitmap，如果是自我赋值，也删除了rhs的bitmap：
    delete pb;
    pb = new Bitmap(*rhs.pb);
    return *this;
  }

 private:
  Bitmap* pb;  // 指针，指向一个从heap分配而得的对象
};
void func() {
  // 自我赋值发生在对象被赋值给自己时：
  Widget w;
  w = w;  //  看着愚蠢，但是合法

  // 潜在的自我赋值:
  // a[i] = a[j];
  // *px = *py;
}
}  // namespace n1

namespace n2 {
class Widget {
 public:
  Widget() : pb(new Bitmap) {}
  Widget& operator=(const Widget& rhs) {
    if (this == &rhs) return *this;  // 证同测试
    delete pb;
    // 这个版本仍然存在异常方面的麻烦，如果new导致异常，
    // Widget最终会持有一个指向一块被删除的Bitmap：
    pb = new Bitmap(*rhs.pb);
    return *this;
  }

 private:
  Bitmap* pb;
};
void func() {
  Widget w;
  w = w;
}
}  // namespace n2

namespace n3 {
class Widget {
 public:
  Widget() : pb(new Bitmap) {}
  Widget& operator=(const Widget& rhs) {
    // 如果没有这句，也能保证自我赋值的安全性，
    // 但如果很关心效率，就应该把证同测试放到开始处：
    if (this == &rhs) return *this;

    // 让operator=具备异常安全性往往自动获得自我赋值安全的回报：
    Bitmap* pOrig = pb;
    pb = new Bitmap(*rhs.pb);  // 如果new发生异常，pb指向不变
    delete pOrig;              // 确定new没有异常后，再删除pb
    return *this;
  }

 private:
  Bitmap* pb;
};
void func() {
  Widget w;
  w = w;
}
}  // namespace n3

namespace n4 {
class Widget {
 public:
  Widget() : pb(new Bitmap) {}
  // 确保代码不但异常安全而且自我赋值安全的一个代替方案是，
  // 使用copy and swap技术，它是个常见而够好的operator=撰写方法：
  void swap(Widget& rhs) {}
  Widget& operator=(const Widget& rhs) {
    Widget temp(rhs);
    swap(temp);
    return *this;
  }

 private:
  Bitmap* pb;
};
void func() {
  Widget w;
  w = w;
}
}  // namespace n4

// 请记住：
// 1.确保当对象自我赋值时operator=有良好行为，其中技术包括：
//   a.比较来源对象和目标对象的地址；
//   b.精心周到的语句顺序；
//   c.copy-and-swap。
// 2.确定任何函数如果操作一个以上的对象，
//   而其中多个对象是同一个对象时，其行为仍然正确。

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
      n2::func();
      break;
    case 2:
      n3::func();
      break;
    case 3:
      n4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}