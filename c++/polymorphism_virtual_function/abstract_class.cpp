#include <iostream>

namespace n1 {
// 包含纯虚函数的类称为抽象类（Abstract Class）。
// 之所以说它抽象，是因为它无法实例化，也就是无法创建对象。
// 因为纯虚函数没有函数体，不是完整的函数，无法调用，也无法为其分配内存空间。
// 抽象类通常是作为基类，让派生类去实现纯虚函数。
// 抽象基类除了约束派生类还可以实现多态，这或许才是C++提供纯虚函数的主要目的。
class Interface {
 public:
  // 在C++中，可以将虚函数声明为纯虚函数。
  // 纯虚函数没有函数体只有声明，在虚函数声明的结尾加上=0，表明为纯虚函数。
  // =0并不表示函数返回值为0，它只起形式上的作用，告诉编译系统这是纯虚函数。
  virtual void show1() = 0;
  virtual void show2() = 0;

  // 抽象基类中除了包含纯虚函数外，
  // 还可以包含其它的成员函数（虚函数或普通函数）和成员变量。
  void show3() { std::cout << "Interface::show3 " << i << std::endl; }
  virtual void show4() { std::cout << "Interface::show4 " << i << std::endl; }

 protected:
  int i = 5;

  // 只有虚函数才能被声明为纯虚函数，普通成员函数不能声明为纯虚函数：
  // void show5() = 0;
};

// 全局函数不能声明为纯虚函数：
// void show6() = 0;

namespace test1 {
class D : public Interface {
 public:
  virtual void show1() { std::cout << "D::show1" << std::endl; }
};
void func() {
  // Interface i;  // 抽象类无法创建对象
  // D1 d1;  // 没有全部实现纯虚函数，也无法创建对象
}
}  // namespace test1
namespace test2 {
class D : public Interface {  // 派生类必须实现纯虚函数才能被实例化
 public:
  // 虽然抽象基类没定义，却强制要求派生类全部定义，这就是抽象基类的霸王条款：
  virtual void show1() { std::cout << "D::show1" << std::endl; }
  virtual void show2() { std::cout << "D::show2" << std::endl; }
};
void func() {
  D d;
  d.show1();  // D::show1
  d.show2();  // D::show2
  d.show3();  // Interface::show3 5
  d.show4();  // Interface::show4 5
}
}  // namespace test2

namespace test3 {
class D : public Interface {
 public:
  virtual void show1() { std::cout << "D::show1" << std::endl; }
  virtual void show2() { std::cout << "D::show2" << std::endl; }
  void show3() { std::cout << "D::show3 " << i << std::endl; }
  void show4() { std::cout << "D::show4 " << i << std::endl; }
};

void func() {
  D d;
  d.show1();  // D::show1
  d.show2();  // D::show2
  d.show3();  // D::show3 5
  d.show4();  // D::show4 5
}
}  // namespace test3
}  // namespace n1

namespace n2 {
// 两个基类有同名的虚函数要实现，怎么办?
class B1 {
 public:
  virtual int get() { return i; }

 protected:
  int i = 1;
};
class B2 {
 public:
  virtual int get() { return i; }

 protected:
  int i = 2;
};

namespace test1 {
class D : public B1, public B2 {
 public:
  virtual int get() {  // 只能实现一个get函数，算B1的，还是B2的？
    // return B1::i;
    // return B2::i;
    return -1;
  }
};
void func() {
  D d;
  B1& b1 = d;
  B2& b2 = d;
  std::cout << d.get() << std::endl;   // -1
  std::cout << b1.get() << std::endl;  // -1
  std::cout << b2.get() << std::endl;  // -1

  D* p = new D();
  B1* pb1 = p;
  B2* pb2 = p;
  std::cout << p->get() << std::endl;    // -1
  std::cout << pb1->get() << std::endl;  // -1
  std::cout << pb2->get() << std::endl;  // -1
}
}  // namespace test1

namespace test2 {
class I1 : public B1 {
 public:
  virtual int get() { return getFromB1(); }

 private:
  virtual int getFromB1() = 0;
};
class I2 : public B2 {
 public:
  virtual int get() { return getFromB2(); }

 private:
  virtual int getFromB2() = 0;
};
class D : public I1, public I2 {
 private:
  virtual int getFromB1() { return B1::i; }
  virtual int getFromB2() { return B2::i; }
};

void func() {
  D d;
  B1& b1 = d;
  B2& b2 = d;
  // std::cout << d.get() << std::endl;   // 编译错误
  std::cout << b1.get() << std::endl;  // 1
  std::cout << b2.get() << std::endl;  // 2

  D* p = new D();
  B1* pb1 = p;
  B2* pb2 = p;
  // std::cout << p->get() << std::endl;    // 编译错误
  std::cout << pb1->get() << std::endl;  // 1
  std::cout << pb2->get() << std::endl;  // 2
}
}  // namespace test2
}  // namespace n2

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::test2::func();
      break;
    case 1:
      n1::test3::func();
      break;
    case 2:
      n2::test1::func();
      break;
    case 3:
      n2::test2::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}
