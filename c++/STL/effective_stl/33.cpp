#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;

// 对包含指针的容器使用remove这一类算法时要特别小心

class Widget {
 public:
  bool isCertified() const {}
};

// 当容器中存放的是指向动态分配的对象的指针的时候，应该避免使用remove和类似的算法（remove_if和unique）。
// 很多情况下，partition是个不错的选择。
// 无法避免对这种容器使用remove，那么一种可以消除该问题的做法是，在进行erase-remove之前，先把指向未被验证的指针置空，然后清除空指针。
void delAndNullifyUncertified(Widget*& pWidget) {
  if (!pWidget->isCertified()) {
    delete pWidget;
    pWidget = 0;
  }
}

// 如果容器中存放的不是普通指针，而是具有引用技术功能的智能指针，那么与remove相关的困难就不再存在了。

int main() {
  vector<Widget*> v;
  v.push_back(new Widget);
  v.push_back(new Widget);
  v.push_back(new Widget);
  v.erase(remove_if(v.begin(), v.end(), mem_fun(&Widget::isCertified)),
          v.end());
  // 条款7中描述，删除容器中的指针并不能删除该指针所指的对象。
  // 担心是正确的，但是已经晚了，在erase被调用之前，remove_if可能已经造成资源泄露了。
  // 要被删除的指针已经被那些不会被删除的指针覆盖了。没有任何指针指向要被删除的指针，永远不会被删除了，资源也不会别释放。

  for_each(v.begin(), v.end(), delAndNullifyUncertified);
  v.erase(remove(v.begin(), v.end(), static_cast<Widget*>(0)), v.end());

  vector<shared_ptr<Widget> > v1;
  v1.push_back(shared_ptr<Widget>(new Widget));

  return 0;
}