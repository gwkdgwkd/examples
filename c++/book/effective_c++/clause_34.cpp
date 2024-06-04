#include <iostream>
#include <string>

// 区分接口继承和实现继承

// public继承概念，有两部分组成：函数接口继承和函数实现继承。
// 就像是函数声明与函数定义之间的差异。
// derived class继承的内容：
// 1.有时希望只继承函数的接口（声明）；
// 2.有时希望继承函数的接口和实现，但希望能够覆写继承的实现；
// 3.有时希望继承函数的接口和实现，并且不允许覆写任何东西。

namespace n1 {
class Shape {
  // Shape是个抽象类，所以不能创建Shape实体，只能创建其derived的实体。
  // 尽管如此，Shape还是强烈影响了所有以public形式继承它的derived class，
  // 因为成员函数的接口总是会被继承。
 public:
  // pure virtual函数有两个最突出的特性：
  // 它们必须被任何继承了它们的class重新声明，而且在抽象class中通常没有定义。
  // 声明一个pure virtual函数的目的是为了让derived class只继承函数接口：
  virtual void draw() const = 0;  // pure virtual

  // impure virtual函数会提供一份实现代码，derived class可能覆写它。
  // 声明impure virtual函数是让derived class继承该函数的接口和缺省实现：
  virtual void error(const std::string& msg) {}  // impure virtual

  // 如果成员函数是non-virtual，意味着并不打算在derived class中有不同的行为。
  // non-virtual函数的不变性凌驾特异性，所以它绝不该在derived中被重新定义。
  // 声明non-virtual函数是为了让derived class继承函数接口及一份强制性实现：
  int objectID() const {}  // non-virtual
};
class Rectangle : public Shape {
  void draw() const { std::cout << "Rectangle::draw" << std::endl; }
};
class Ellipse : public Shape {
  void draw() const { std::cout << "Ellipse::draw" << std::endl; }
};

// 可以为pure virtual函数提供定义，C++并不会发出怨言，
// 但调用它的唯一途径是调用时明确指出其class名称：
void Shape::draw() const { std::cout << "Shape::draw" << std::endl; }
// 除了给大师程序员一个深刻印象外，一般而言这项性质用途有限。
// 但它可以实现一种机制：为简朴的impure virtual函数提供更平常更安全的缺省实现。

void func() {
  // Shape* ps = new Shape;  // 错误
  Shape* ps1 = new Rectangle;
  ps1->draw();
  Shape* ps2 = new Ellipse;
  ps2->draw();
  ps1->Shape::draw();
  ps2->Shape::draw();
  // Rectangle::draw
  // Ellipse::draw
  // Shape::draw
  // Shape::draw
}
}  // namespace n1

namespace n2 {
// 声明impure virtual函数同时指定函数声明和函数缺省行为，有可能造成危险：
class Airport {};
namespace test1 {
class Airplane {
 public:
  virtual void fly(const Airport& destination);
};
void Airplane::fly(const Airport& destination) {
  std::cout << "Airplane fly" << std::endl;
}
class ModelA : public Airplane {};  // 使用默认的fly
class ModelB : public Airplane {};  // 使用默认的fly
class ModelC : public Airplane {};  // 忘记声明fly函数，调用默认的fly

void func() {
  Airport ap;
  Airplane* pa = new ModelC;
  pa->fly(ap);  // Airplane fly
}
}  // namespace test1

namespace test2 {
// 问题不在于有缺省行为而在于：
// ModelC在未明确说明要的情况下就继承了该缺省行为。
// 切断virtual函数接口和其缺省实现之间的连接的伎俩：
class Airplane {
 public:
  virtual void fly(const Airport& destination) = 0;

 protected:
  void defaultFly(const Airport& destination) {
    // non-virtual函数，任何一个derived class都不应该重新定义
    std::cout << "Airplane defaultFly" << std::endl;
  }
};
class ModelA : public Airplane {
 public:
  virtual void fly(const Airport& destination) { defaultFly(destination); }
};
class ModelB : public Airplane {
 public:
  virtual void fly(const Airport& destination) { defaultFly(destination); }
};
class ModelC : public Airplane {
 public:
  // 现在ModelC不可能意外继承不正确的flay实现了，因为如果不自己实现，编译不通过：
  virtual void fly(const Airport& destination) {
    std::cout << "ModelC fly" << std::endl;
  }
};
void func() {
  Airport ap;
  Airplane* pa = new ModelC;
  pa->fly(ap);  // ModelC fly
}
}  // namespace test2

namespace test3 {
// 有人反对像上面一样以不同的函数分别提供接口和缺省实现。
// 关心过度雷同的函数名引起class命名空间污染问题。
// 但是他们也同意，接口和缺省实现应该分开。
// 可以利用pure virtual函数必须在derived class中重新声明，
// 但它们也可以有自己的实现这一事实。
class Airplane {
 public:
  virtual void fly(const Airport& destination) = 0;
};
void Airplane::fly(const Airport& destination) {
  // 合并fly和defaultFly，就丧失了让两个函数享有不同保护级别的机会，
  // 实际上被protected的函数如今成了public。
  std::cout << "Airplane fly" << std::endl;
}
class ModelA : public Airplane {
 public:
  virtual void fly(const Airport& destination) { Airplane::fly(destination); }
};
class ModelB : public Airplane {
 public:
  virtual void fly(const Airport& destination) { Airplane::fly(destination); }
};
class ModelC : public Airplane {
 public:
  virtual void fly(const Airport& destination) {
    std::cout << "ModelC fly" << std::endl;
  }
};
void func() {
  Airport ap;
  Airplane* p1 = new ModelA;
  p1->fly(ap);  // Airplane fly

  Airplane* p2 = new ModelB;
  p2->fly(ap);  // Airplane fly

  Airplane* p3 = new ModelC;
  p3->fly(ap);  // ModelC fly
}
}  // namespace test3
}  // namespace n2

// 声明成员函数时，必须谨慎选择，常犯的两个错误：
// 1.将所有函数声明为non-virtual，使得derived class没有能力做特化工作，
//   non-virtual析构函数尤其会带来问题；
// 2.将所有成员函数声明为virtual，有些时候是正确的，
//   也有可能是class设计者缺乏坚定的立场，某些函数就不该在derived class中被重新定义。

// 请记住：
// 1.接口继承和实现继承不同；
// 2.在public继承之下，derived class总是继承base class的接口；
// 3.pure virtual函数只是具体指定接口继承；
// 4.简朴的（非纯）impure virtual函数具体指定接口继承及缺省实现继承；
// 5.non-virtual函数具体指定接口继承以及强制性实现继承。

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
      n2::test1::func();
      break;
    case 2:
      n2::test2::func();
      break;
    case 3:
      n2::test3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}