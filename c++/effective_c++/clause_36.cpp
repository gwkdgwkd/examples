#include <iostream>

using namespace std;

// 绝不重新定义继承而来的non-virtual函数

class B {
 public:
  void mf() { cout << "B::mf" << endl; };
  virtual void vf() { cout << "B::vf" << endl; };
};
class D : public B {};
class E : public B {
 public:
  void mf() { cout << "E::mf" << endl; };  // 遮掩了B::mf
  void vf() { cout << "E::vf" << endl; };
};

// non-virtual函数是静态绑定。
// virtual函数是动态绑定。

// 适用与B对象的每一件事，也适用于D对象，因为每个D对象都是一个B对象；
// B的derived class一定会继承mf的接口和实现，
// 因为mf是B的一个non-virtual函数。
// 如果D重新定义mf，那么设计就会出现矛盾。

// 请记住：
// 绝对不要重新定义继承而来的non-virtual函数。

int main() {
  D x;
  B* pB = &x;
  pB->mf();  // B::mf
  D* pD = &x;
  pD->mf();  // B::mf

  E e;
  B* pF = &e;
  pF->mf();  // B::mf
  pF->vf();  // E::vf
  E* pE = &e;
  pE->mf();  // E::mf
  pE->vf();  // E::vf

  return 0;
}