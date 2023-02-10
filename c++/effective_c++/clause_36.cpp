#include <iostream>

// 绝不重新定义继承而来的non-virtual函数

class B {
 public:
  void mf() { std::cout << "B::mf" << std::endl; };
  virtual void vf() { std::cout << "B::vf" << std::endl; };
};
class D : public B {};
class E : public B {
 public:
  void mf() { std::cout << "E::mf" << std::endl; };  // 遮掩了B::mf
  void vf() { std::cout << "E::vf" << std::endl; };
};

// non-virtual函数是静态绑定，virtual函数是动态绑定。

// 适用与B对象的每一件事，也适用于D对象，因为每个D对象都是一个B对象；
// B的derived一定会继承mf的接口和实现，因为mf是B的一个non-virtual函数。
// 如果D重新定义mf，那么设计就会出现矛盾。

// 请记住：
// 1.绝对不要重新定义继承而来的non-virtual函数。

int main() {
  D d;
  B* pB = &d;
  pB->mf();  // B::mf
  D* pD = &d;
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