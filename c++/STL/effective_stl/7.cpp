#include <algorithm>
#include <deque>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

// 如果容器中包括了通过new创建的指针，切记在容器对象析构前将指针delete掉

// 指针容器在自己被析构时会析构所包含的每个元素，但指针的“析构函数”不做任何事情，当然也不会调用delete。
class Widget {};
void doSomething() {  // 导致资源泄露
  vector<Widget *> vwp;
  for (int i = 0; i < 10; ++i) {
    vwp.push_back(new Widget);
  }
}

// 简单的做法是：
void doSomething1() {
  vector<Widget *> vwp;
  for (int i = 0; i < 10; ++i) {
    vwp.push_back(new Widget);
  }
  for (vector<Widget *>::iterator i = vwp.begin(); i != vwp.end(); ++i) {
    delete *i;
  }
  // 这段代码可以删除，但是不是异常安全的
  // 如果在填充指针和删除指针的两个过程中有异常抛出，同样会资源泄露。幸运的是，这两个问题都可以克服
}

template <typename T>
struct DeleteObject : public unary_function<const T *, void> {
  void operator()(const T *ptr) const { delete ptr; }
};
void doSomething2() {
  vector<Widget *> vwp;
  for (int i = 0; i < 10; ++i) {
    vwp.push_back(new Widget);
  }
  for_each(vwp.begin(), vwp.end(), DeleteObject<Widget>());
}

// 从没有虚析构函数的类进行公有继承是C++的一项重要禁忌
class SpecialString : public string {};  // string没有虚析构函数
void doSomething3() {
  deque<SpecialString *> dssp;
  for_each(dssp.begin(), dssp.end(), DeleteObject<string>());
  // 不确定的行为。通过基类的指针删除派生类对象，而基类又没有虚析构函数
}

// 通过让编译器推断出传给DeleteObject::operator()的指针的类型，可以消除这个错误
struct DeleteObject1 {
  template <typename T>
  void operator()(const T *ptr) const {
    delete ptr;
  }
};
void doSomething4() {
  deque<SpecialString *> dssp;
  for (int i = 0; i < 10; ++i) {
    dssp.push_back(new SpecialString);
  }
  for_each(dssp.begin(), dssp.end(), DeleteObject1());
  // 直接而类型安全，正是我们所希望的方式
  // 仍然不是异常安全的。如果在SpecialString被创建而for_each的调用还没有开始时有异常抛出，则会资源泄露。
  // 有很多方式类解决这个问题，最简单的是用智能指针代替指针容器。
}

void doSomething5() {
  vector<shared_ptr<Widget>> vwp;
  for (int i = 0; i < 10; ++i) {
    vwp.push_back(shared_ptr<Widget>(new Widget));
  }
  // 不会有Widget资源泄露，即使上面的代码有异常被抛出
  // 永远都不要错误的认为：可以通过创建auto_ptr的容器使指针被自动删除。这个想法很可怕，也很危险。
}

int main() { return 0; }