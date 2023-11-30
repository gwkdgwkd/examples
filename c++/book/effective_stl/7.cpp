#include <algorithm>
#include <deque>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// 如果容器中包括了通过new创建的指针，切记在容器对象析构前将指针delete掉

class Widget {};

namespace n1 {
// 指针容器在自己被析构时会析构所包含的每个元素，
// 但指针的析构函数不做任何事情，当然也不会调用delete。
void func() {  // 导致资源泄露
  std::vector<Widget *> vwp;
  for (int i = 0; i < 10; ++i) {
    vwp.push_back(new Widget);
  }
}
}  // namespace n1

namespace n2 {
// 简单的做法是：
void func() {
  std::vector<Widget *> vwp;
  for (int i = 0; i < 10; ++i) {
    vwp.push_back(new Widget);
  }
  for (std::vector<Widget *>::iterator i = vwp.begin(); i != vwp.end(); ++i) {
    delete *i;
  }
  // 可以删除但是不是异常安全的，如果在填充指针和删除指针的两个过程中有异常抛出，
  // 同样会资源泄露，幸运的是，这两个问题都可以克服。
}
}  // namespace n2

template <typename T>
struct DeleteObject : public std::unary_function<const T *, void> {
  void operator()(const T *ptr) const { delete ptr; }
};

namespace n3 {
void func() {
  std::vector<Widget *> vwp;
  for (int i = 0; i < 10; ++i) {
    vwp.push_back(new Widget);
  }
  std::for_each(vwp.begin(), vwp.end(), DeleteObject<Widget>());
}
}  // namespace n3

class SpecialString : public std::string {};  // string没有虚析构函数
namespace n4 {
// 从没有虚析构函数的类进行公有继承是C++的一项重要禁忌：
void func() {
  std::deque<SpecialString *> dssp;
  std::for_each(dssp.begin(), dssp.end(), DeleteObject<std::string>());
  // 不确定的行为，通过基类的指针删除派生类对象，而基类又没有虚析构函数
}
}  // namespace n4

namespace n5 {
// 让编译器推断出传给DeleteObject::operator()的指针的类型，可以消除这个错误：
struct DeleteObject {
  template <typename T>
  void operator()(const T *ptr) const {
    delete ptr;
  }
};
void func() {
  std::deque<SpecialString *> dssp;
  for (int i = 0; i < 10; ++i) {
    dssp.push_back(new SpecialString);
  }
  for_each(dssp.begin(), dssp.end(), DeleteObject());
  // 直接而类型安全，正是所希望的方式，仍然不是异常安全的。
  // 如果在SpecialString被创建，而for_each的调用还没有开始时有异常抛出，
  // 则会资源泄露，有很多方式类解决这个问题，最简单的是用智能指针代替指针容器。
}
}  // namespace n5

namespace n6 {
void func() {
  std::vector<std::shared_ptr<Widget>> vwp;
  for (int i = 0; i < 10; ++i) {
    vwp.push_back(std::shared_ptr<Widget>(new Widget));
  }
  // 不会有Widget资源泄露，即使上面的代码有异常被抛出。
  // 永远都不要认为：可以通过创建auto_ptr的容器使指针被自动删除。
  // 这个想法是错误的，很可怕，也很危险。
}
}  // namespace n6

int main() { return 0; }