#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

// 切勿创建包含auto_ptr的容器对象

// 当你拷贝一个auto_ptr时，
// 它所指向的对象的所有权被移交到拷入的auto_ptr上，而它自身被置为NULL。
// 拷贝一个auto_ptr意味着改变它的值。
class Widget {
 public:
  bool operator<(const Widget& rhs) { return false; }
};
bool widgetAPCompare(const std::auto_ptr<Widget>& lhs,
                     const std::auto_ptr<Widget>& rhs) {
  return *lhs < *rhs;
}

int main() {
  std::auto_ptr<Widget> pw1(new Widget);
  // pw2指向pw1的Widget；pw1被置为NULL：
  std::auto_ptr<Widget> pw2(pw1);
  // 现在pw1又指向Widget了；pw2被置为NULL：
  pw1 = pw2;

  std::vector<std::auto_ptr<Widget> > widgets;
  std::sort(widgets.begin(), widgets.end(), widgetAPCompare);
  // 在排序过程中，widgets中的一个或多个auto_ptr可能被置为NULL。
  // 对vector所做的排序操作可能会改变它的内容。
  // sort的方法使用了快速排序算法的一个变种，在排序过程中会有拷贝操作。
  // 千万别创建包含auto_ptr的容器，即使你的STL平台允许这样做。

  return 0;
}