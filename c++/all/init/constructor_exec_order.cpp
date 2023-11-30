#include <iostream>

class MemA {
 public:
  MemA() { std::cout << "MemA()" << std::endl; }
  MemA(int) { std::cout << "MemA(int)" << std::endl; }
  MemA(int, int) { std::cout << "MemA(int,int)" << std::endl; }
  ~MemA() { std::cout << "~MemA" << std::endl; }
};

class MemB {
 public:
  MemB() { std::cout << "MemB()" << std::endl; }
  MemB(int) { std::cout << "MemB(int)" << std::endl; }
  ~MemB() { std::cout << "~MemB" << std::endl; }
};

class MemC {
 public:
  MemC() { std::cout << "MemC()" << std::endl; }
  MemC(int) { std::cout << "MemC(int)" << std::endl; }
  ~MemC() { std::cout << "~MemC" << std::endl; }
};

class Base {
 public:
  Base() { std::cout << "Base()" << std::endl; }
  Base(int) { std::cout << "Base(int)" << std::endl; }
  ~Base() { std::cout << "~Base" << std::endl; }
};

class BaseA : public Base {
 public:
  BaseA() { std::cout << "BaseA()" << std::endl; }
  BaseA(int i) { std::cout << "BaseA(int)" << std::endl; }
  BaseA(int i, int j) : Base(j) { std::cout << "BaseA(int,int)" << std::endl; }
  ~BaseA() { std::cout << "~BaseA" << std::endl; }
};

class BaseB {
 public:
  BaseB() { std::cout << "BaseB()" << std::endl; }
  BaseB(int i) { std::cout << "BaseB(int)" << std::endl; }
  ~BaseB() { std::cout << "~BaseB" << std::endl; }
};

namespace n1 {
// 基类的成员函数可以被继承，可以通过派生类的对象访问，
// 但这仅仅指的是普通的成员函数，类的构造函数不能被继承。
// 构造函数不能被继承是有道理的，因为它的名字和派生类的名字也不一样，
// 不能成为派生类的构造函数，当然更不能成为普通的成员函数。
// 和构造函数类似，析构函数也不能被继承，与构造函数不同的是，
// 在派生类的析构函数中不用显式地调用基类的析构函数，
// 因为每个类只有一个析构函数，编译器知道如何选择，无需程序员干涉。

// 多继承下的构造函数和单继承基本相同，只是在派生类构造函数中调用多个基类的构造函数。
// 基类构造函数的调用顺序和和它们在派生类构造函数中出现的顺序无关，
// 而是和声明派生类时基类出现的顺序相同。
class Derived : public BaseA, /*Base 0, BaseA 1*/ public BaseB /*BaseB 2*/ {
 public:
  Derived() { std::cout << "Derived()" << std::endl; /*Derived 7*/ }

  // 封闭类对象生成时，先执行所有成员对象的构造函数，然后才执行封闭类自己的构造函数。
  // 成员对象构造函数的执行顺序和成员对象在定义的顺序一致，与初始化列表中出现的顺序无关。
  Derived(int i, int j, int k, int l, int m, int n)
      : BaseA(i), BaseB(j), a1(k), b(l), c(m), a2(m, n) {
    std::cout << "Derived(int,int,int,int,int,int)" << std::endl; /*Derived 7*/
  }

  // 由于大部分基类都有private的成员变量，在派生类中无法访问，
  // 所以对于一些继承过来的成员变量的初始化工作无法在派生类中完成。
  // 解决这个问题的办法是，在派生类的构造函数中调用基类的构造函数。
  // 可以将基类构造函数的调用放在参数初始化表前面或者后面，不管它们的顺序如何，
  // 派生类构造函数总是先调用基类构造函数再执行其他代码（参数初始化表以及函数体中的代码）。
  // 基类构造函数永远是先执行的，与在初始化列表中的顺序无关。
  Derived(int i, int j, int k, int l, int m)
      : a2(m, m), c(m), BaseB(i), b(l), BaseA(j), a1(k) {
    std::cout << "Derived(int,int,int,int,int)" << std::endl; /*Derived 7*/
  }

  // 通过派生类创建对象时必须要调用基类的构造函数，这是语法规定。
  // 换句话说，定义派生类构造函数时最好指明基类构造函数，
  // 如果不指明，就调用基类的默认构造函数（不带参数的构造函数）；
  // 如果基类没有默认构造函数，那么编译失败。
  Derived(int i, int j, int k) : c(i), BaseB(j), b(k) {
    std::cout << "Derived(int,int,int)" << std::endl; /*Derived 7*/
  }

  // 派生类构造函数中只能调用直接基类的构造函数，不能调用间接基类的。
  // 构造函数的调用顺序是按照继承的层次自顶向下、从基类再到派生类的。
  // C++这样规定是有道理的，因为在Derived中调用了BaseA的构造函数，
  // BaseA又调用了Base的构造函数，相当于Derived间接地调用了Base的构造函数，
  // 如果再在Derived中显式地调用Base的构造函数，那么Base的构造函数就被调用了两次，
  // 相应地，初始化工作也做了两次，这不仅是多余的，还会浪费CPU时间以及内存，
  // 毫无益处，所以C++禁止在Derived中显式地调用Base的构造函数。
  // Derived(int i) : Base(i) {}
  // error: type ‘Base’ is not a direct base of ‘n1::Derived’

  ~Derived() { std::cout << "~Derived" << std::endl; }

 private:
  // 一个类的成员变量如果是另一个类的对象，就称之为成员对象，包含成员对象的类叫封闭类。
  // 创建封闭类的对象时，它包含的成员对象也需要被创建，这就会引发成员对象构造函数的调用。
  // 成员对象到底是用哪个构造函数初始化的呢？需要借助封闭类构造函数的初始化列表。
  // 生成封闭类对象的语句一定要让编译器明白其成员对象是如何初始化的，否则就会编译错误。
  MemA a1; /*MemA 3*/
  MemB b;  /*MemB 4*/
  MemC c;  /*MemC 5*/
  MemA a2; /*MemA 6*/
};

void func1() {
  Derived d;
  // Base()
  // BaseA()
  // BaseB()
  // MemA()
  // MemB()
  // MemC()
  // MemA()
  // Derived()
  // ~Derived
  // ~MemA
  // ~MemC
  // ~MemB
  // ~MemA
  // ~BaseB
  // ~BaseA
  // ~Base
}

void func2() {
  Derived d(1, 2, 3, 4, 5, 6);
  // Base
  // BaseA(int)
  // BaseB(int)
  // MemA(int)
  // MemB(int)
  // MemC(int)
  // MemA(int,int)
  // Derived(int,int,int,int,int,int)
  // ~Derived
  // ~MemA
  // ~MemC
  // ~MemB
  // ~MemA
  // ~BaseB
  // ~BaseA
  // ~Base
}

void func3() {
  Derived d(1, 2, 3, 4, 5);
  // Base()
  // BaseA(int)
  // BaseB(int)
  // MemA(int)
  // MemB(int)
  // MemC(int)
  // MemA(int,int)
  // Derived(int,int,int,int,int)
  // ~Derived
  // ~MemA
  // ~MemC
  // ~MemB
  // ~MemA
  // ~BaseB
  // ~BaseA
  // ~Base
}
void func4() {
  Derived d(1, 2, 3);
  // Base()
  // BaseA()
  // BaseB(int)
  // MemA()
  // MemB(int)
  // MemC(int)
  // MemA()
  // Derived(int,int,int)
  // ~Derived
  // ~MemA
  // ~MemC
  // ~MemB
  // ~MemA
  // ~BaseB
  // ~BaseA
  // ~Base
}
}  // namespace n1

namespace n2 {
class Derived : public BaseB /*BaseB 0*/, public BaseA /*Base 1, BaseA 2*/ {
 public:
  Derived(int i, int j, int k, int l, int m, int n)
      : BaseA(i), BaseB(j), a1(k), b(l), c(m), a2(m, n) {
    std::cout << "Derived(int,int,int,int,int,int)" << std::endl; /*Derived 7*/
  }
  ~Derived() { std::cout << "~Derived" << std::endl; }

 private:
  MemA a2; /*MemA 3*/
  MemC c;  /*MemC 4*/
  MemB b;  /*MemB 5*/
  MemA a1; /*MemA 6*/
};

void func() {
  Derived(1, 2, 3, 4, 5, 6);
  // BaseB(int)
  // Base()
  // BaseA(int)
  // MemA(int,int)
  // MemC(int)
  // MemB(int)
  // MemA(int)
  // Derived(int,int,int,int,int,int)
  // ~Derived
  // ~MemA
  // ~MemB
  // ~MemC
  // ~MemA
  // ~BaseA
  // ~Base
  // ~BaseB
}
}  // namespace n2

// 总结：
// 1.派生类优先调用基类构造函数
//   a.在派生类构造函数初始化列表中显式调用基类构造函数；
//   b.如果没有显式调用，那么调用基类默认构造函数（没有参数的）；
//   c.派生类构造函数初始化列表中只能调用直接基类的构造函数，不能调用间接基类的构造函数；
//   d.多继承时基类构造函数调用顺序与声明派生类时的顺序相同，与初始化列表中的顺序无关。
// 2.随后调用成员对象的构造函数
//   a.在类构造函数初始化列表中显式调用成员对象的构造函数；
//   b.如果没有显式调用，那么调用成员对象默认构造函数（没有参数的）；
//   c.成员对象构造函数的调用顺序与类中声明变量的顺序相同，与初始化列表中的顺序无关。
//     建议类中的成员变量声明的顺序和初始化列表中初始化的顺序一致。
// 3.最后执行类构造函数体中的代码
// 4.析构函数
//   a.每个类只有一个析构函数，编译器知道如何选择，无需程序员干涉；
//   b.析构函数调用的顺序与构造函数相反。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 4]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func1();
      break;
    case 1:
      n1::func2();
      break;
    case 2:
      n1::func3();
      break;
    case 3:
      n1::func4();
      break;
    case 4:
      n2::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}