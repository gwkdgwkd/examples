#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

// 对包含指针的容器使用remove这一类算法时要特别小心

class Widget {
 public:
  Widget(int i = 0) : i_(i) { std::cout << "Widget " << i_ << std::endl; }
  ~Widget() { std::cout << "~Widget " << i_ << std::endl; }
  bool isCertified() const { return i_ > 2; }

 private:
  int i_;
};

namespace n1 {
void func() {
  // 当容器中存放的是指向动态分配的对象的指针的时候，
  // 应该避免使用remove和类似的算法remove_if和unique。

  std::vector<Widget*> v{new Widget(1), new Widget(2), new Widget(8),
                         new Widget(9)};
  // Widget 1
  // Widget 2
  // Widget 8
  // Widget 9

  v.erase(
      std::remove_if(v.begin(), v.end(), std::mem_fun(&Widget::isCertified)),
      v.end());

  std::for_each(v.begin(), v.end(), [](Widget* w) {
    delete w;
    w = nullptr;
  });
  // ~Widget 1
  // ~Widget 2

  // 条款7中描述，删除容器中的指针并不能删除该指针所指的对象，担心是正确的，
  // 但是已经晚了，在erase被调用之前，remove_if可能已经造成资源泄露了。
  // 要被删除的指针已经被那些不会被删除的指针覆盖了，没有任何指针指向要被删除的指针，
  // 永远不会被删除了，资源也不会别释放。
}
}  // namespace n1

namespace n2 {
// 很多情况下，partition是个不错的选择，无法避免对这种容器使用remove，
// 那么一种可以消除该问题的做法是，在进行erase-remove之前，
// 先把指向未被验证的指针置空，然后清除空指针。
void delAndNullifyUncertified(Widget*& pWidget) {
  if (pWidget->isCertified()) {
    delete pWidget;
    pWidget = 0;
  }
}

void func() {
  std::vector<Widget*> v{new Widget(1), new Widget(2), new Widget(8),
                         new Widget(9)};
  // Widget 1
  // Widget 2
  // Widget 8
  // Widget 9

  std::for_each(v.begin(), v.end(), delAndNullifyUncertified);
  // ~Widget 8
  // ~Widget 9

  v.erase(remove(v.begin(), v.end(), static_cast<Widget*>(0)), v.end());
  std::for_each(v.begin(), v.end(), [](Widget* w) {
    delete w;
    w = nullptr;
  });
  // ~Widget 1
  // ~Widget 2
}
}  // namespace n2

namespace n3 {
// 如果容器中存放的不是普通指针，而是具有引用技术功能的智能指针，
// 那么与remove相关的困难就不再存在了。

void func() {
  std::vector<std::shared_ptr<Widget>> v;
  v.push_back(std::make_shared<Widget>(1));
  v.push_back(std::make_shared<Widget>(2));
  v.push_back(std::make_shared<Widget>(8));
  v.push_back(std::make_shared<Widget>(9));
  // Widget 1
  // Widget 2
  // Widget 8
  // Widget 9

  v.erase(std::remove_if(
              v.begin(), v.end(),
              [](std::shared_ptr<Widget> p) { return p->isCertified(); }),
          v.end());
  // ~Widget 8
  // ~Widget 9

  std::cout << "after erase" << std::endl;
  // after erase
  // ~Widget 1
  // ~Widget 2
}
}  // namespace n3

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
  return 0;
}