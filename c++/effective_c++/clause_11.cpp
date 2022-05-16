#include <iostream>

using namespace std;

// 在operator=中处理自我赋值

// 自我赋值发生在对象被赋值给自己时
class Bitmap {};
class Widget {
 public:
#if 0
  // 下面的代码表面看起来合理，但自我赋值出现时并不安全
  Widget operator=(const Widget& rhs) {
    delete pb;  // 删除当前使用的bitmap，如果是自我赋值，也删除了rhs的bitmap
    pb = new Bitmap(*rhs.pb);
    return *this;
  }
#endif
#if 0
  Widget operator=(const Widget& rhs) {
    if (this == &rhs) return *this;  // 证同测试
    delete pb;
    // 这个版本仍然存在异常方面的麻烦
    // 如果new导致异常，Widget最终会持有一个指向一块被删除的Bitmap
    pb = new Bitmap(*rhs.pb);
    return *this;
  }
#endif
#if 0
  // 让operator=具备异常安全性往往自动获得自我赋值安全的回报
  // 如果很关心效率，也可以把“证同测试”放到开始处
  Widget operator=(const Widget& rhs) {
    if (this == &rhs) return *this;  // 证同测试，提高效率
    Bitmap* pOrig = pb;
    pb = new Bitmap(*rhs.pb);
    delete pOrig;
    return *this;
  }
#endif

  // 确保代码不但“异常安全”而且“自我赋值安全”的一个代替方案是，使用所谓的copy and swap技术
  // 它是一个常见而够好的operator=撰写方法
  void swap(Widget& rhs){};
  Widget operator=(const Widget& rhs) {
    Widget temp(rhs);
    swap(temp);
    return *this;
  }

 private:
  Bitmap* pb;  // 指针，指向一个从heap分配而得的对象
};

// 请记住：
// 确保当对象自我赋值时operator=有良好行为。
// 其中技术包括比较“来源对象”和“目标对象”的地址、精心周到的语句顺序、以及copy-and-swap。
// 确定任何函数如果操作一个以上的对象，而其中多个对象是同一个对象时，其行为仍然正确。

int main() {
  Widget w;
  w = w;  // 看起来愚蠢，但是合法。

  // 潜在的自我赋值:
  // a[i] = a[j];
  // *px = *py;

  return 0;
}