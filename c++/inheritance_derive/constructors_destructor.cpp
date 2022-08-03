#include <iostream>

// 基类的成员函数可以被继承，可以通过派生类的对象访问，
// 但这仅仅指的是普通的成员函数，类的构造函数不能被继承。
// 构造函数不能被继承是有道理的，因为即使继承了，它的名字和派生类的名字也不一样，
// 不能成为派生类的构造函数，当然更不能成为普通的成员函数。

// 和构造函数类似，析构函数也不能被继承。
// 与构造函数不同的是，在派生类的析构函数中不用显式地调用基类的析构函数，
// 因为每个类只有一个析构函数，编译器知道如何选择，无需程序员干涉。

// 派生类构造函数中只能调用直接基类的构造函数，不能调用间接基类的。
// A --> B --> C
// A类构造函数 --> B类构造函数 --> C类构造函数
// 构造函数的调用顺序是按照继承的层次自顶向下、从基类再到派生类的。
// C++这样规定是有道理的，因为在C中调用了B的构造函数，B又调用了A的构造函数，
// 相当于C间接地（或者说隐式地）调用了A的构造函数，
// 如果再在C中显式地调用A的构造函数，那么A的构造函数就被调用了两次，
// 相应地，初始化工作也做了两次，这不仅是多余的，还会浪费CPU时间以及内存，
// 毫无益处，所以C++禁止在C中显式地调用A的构造函数。

namespace n1 {
// 对继承过来的成员变量的初始化工作也要由派生类的构造函数完成，
// 但大部分基类都有private的成员变量，在派生类中无法访问，
// 更不能使用派生类的构造函数来初始化。
// 这种矛盾在C++继承中是普遍存在的，
// 解决这个问题的思路是：在派生类的构造函数中调用基类的构造函数。
// 可以将基类构造函数的调用放在参数初始化表前面或者后面，
// 不管它们的顺序如何，派生类构造函数总是先调用基类构造函数再执行其他代码，
// 包括参数初始化表以及函数体中的代码。
// 基类构造函数总是被优先调用，这说明创建派生类对象时，
// 会先调用基类构造函数，再调用派生类构造函数。

class B {
 public:
  B(int i) : i_(i) { std::cout << "B(int)" << std::endl; }

 private:
  int i_;
};

class D1 : public B {
 public:
  D1(int i, int j) : B(i), j_(i) { std::cout << "D1(int, int)" << std::endl; }

 private:
  int j_;
};

class D2 : public B {
 public:
  D2(int i, int j) : j_(i), B(i) { std::cout << "D2(int, int)" << std::endl; }

 private:
  int j_;
};

void testN1() {
  D1 d1(4, 5);
  // B(int)
  // D1(int, int)

  D2 d2(7, 8);
  // B(int)
  // D2(int, int)

  // 基类构造函数永远是先执行的，与在初始化列表中的顺序无关
}
}  // namespace n1

namespace n2 {
// 通过派生类创建对象时必须要调用基类的构造函数，这是语法规定。
// 换句话说，定义派生类构造函数时最好指明基类构造函数；
// 如果不指明，就调用基类的默认构造函数（不带参数的构造函数）；
// 如果没有默认构造函数，那么编译失败。

class B {
 public:
  B() : i_(6) { std::cout << "B()" << std::endl; }
  B(int i) : i_(i) { std::cout << "B(int)" << std::endl; }

 private:
  int i_;
};

class D : public B {
 public:
  D() { std::cout << "D()" << std::endl; }
  D(int i) : B(i) { std::cout << "D(int)" << std::endl; }
  D(float i) { std::cout << "D(float)" << std::endl; }
  D(int i, int j) : B(i), j_(j) { std::cout << "D(int, int)" << std::endl; }

 private:
  int j_;
};

// 基类没有默认构造函数，派生类又没指定调用B(int)构造函数，编译失败：
// no matching function for call to ‘n2::test2::B::B()’
// class B {
//  public:
//   B(int i) : i_(i) { std::cout << "B(int)" << std::endl; }
//  private:
//   int i_;
// };
// class D : public B {
//  public:
//   D(int i, int j) : j_(j) { std::cout << "D(int, int)" << std::endl; }
//  private:
//   int j_;
// };

void testN2() {
  D d1;
  // B()
  // D()

  D d2(5);
  // B(int)
  // D(int)

  D d3(3.4f);
  // B()
  // D(float)

  D d4(1, 2);
  // B(int)
  // D(int, int)
}
}  // namespace n2

namespace n3 {
// 析构函数的执行顺序和构造函数的执行顺序也刚好相反：
// 1.创建派生类对象时，构造函数的执行顺序和继承顺序相同，
//   即先执行基类构造函数，再执行派生类构造函数。
// 2.而销毁派生类对象时，析构函数的执行顺序和继承顺序相反，
//   即先执行派生类析构函数，再执行基类析构函数。

class A {
 public:
  A() { std::cout << "A constructor" << std::endl; }
  ~A() { std::cout << "A destructor" << std::endl; }
};
class B : public A {
 public:
  B() { std::cout << "B constructor" << std::endl; }
  ~B() { std::cout << "B destructor" << std::endl; }
};
class C : public B {
 public:
  C() { std::cout << "C constructor" << std::endl; }
  ~C() { std::cout << "C destructor" << std::endl; }
};

void testN3() {
  C c;
  // A constructor
  // B constructor
  // C constructor
  // C destructor
  // B destructor
  // A destructor
}
}  // namespace n3

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::testN1();
      break;
    case 1:
      n2::testN2();
      break;
    case 2:
      n3::testN3();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}
