#include <iostream>

namespace n1 {
// C++继承中重载、重写、重定义的区别：
// 1.成员函数重载（overload）的特征：
//   a.相同的范围（在同一个类中）；
//   b.函数名字相同；
//   c.参数不同；
//   d.不能靠返回类型来判断；
//   e.virtual关键字可有可无。
// 2.重写（override，覆盖）是指子类重新定义父类中有相同名称和参数的虚函数：
//   a.不同的范围，分别位于基类和派生类中；
//   b.函数的名字相同；
//   c.参数相同；
//   d.类型相同；
//   e.基类函数必须有virtual关键字，不能是static的；
//   f.重写函数的访问修饰符可以不同，尽管基类的virtual是private的，
//     派生类中也可以改写为public，protected的。
// 3.重定义（redefining，隐藏）是指子类重新定义父类中名称相同的非虚函数：
//   a.如果派生类的函数和基类的函数同名，但是参数不同，
//     此时，不管有无virtual，基类的函数被隐藏；
//   b.如果派生类的函数与基类的函数同名，并且参数也相同：
//     (1).基类函数没有virtual关键字，基类的函数被隐藏；
//     (2).基类函数有virtual关键字，那么是重写不是重定义。

class Base {
 private:
  virtual void display() { std::cout << "Base display()" << std::endl; }
  void say() { std::cout << "Base say()" << std::endl; }

 public:
  void exec() {
    display();
    say();
  }
  // overload，两个f1函数在Base类的内部被重载：
  void f1(std::string a) { std::cout << "Base f1(string)" << std::endl; }
  void f1(int a) { std::cout << "Base f1(int)" << std::endl; }
};

class Derive : public Base {
 public:
  // override，基类中display为虚函数，故此处为重写：
  void display() { std::cout << "Derive display()" << std::endl; }
  // overload，重载，display()没有重写，display(int)也是重载：
  void display(int i) { std::cout << "Derive display(int)" << std::endl; }
  // redefining，f1函数在Base类中不为虚函数，故此处为重定义：
  void f1(int a, int b) { std::cout << "Derive f1(int,int)" << std::endl; }
  // redefining，同上：
  void say() { std::cout << "Derive say()" << std::endl; }
};

void func() {
  Derive a;
  Base *b = &a;

  // b里边的函数display被Derive类覆盖，但是say还是使用基类的：
  b->exec();
  // Derive display()
  // Base say()

  a.exec();
  // Derive display()
  // Base say()
  // say为啥也是调用基类的？
  // exec是基类的函数，exec中的this表示的是基类对象，
  // 所以在exec中通过this调用的say也是基类的，是这么解释么？

  a.say();  // Derive say()

  b->f1("niaho");  // Base f1(string)
  // a.f1("nihao");

  a.f1(5, 6);  // Derive f1(int,int)
}
}  // namespace n1

namespace n2 {
// 在通常情况下，一旦在基类A中的成员函数f被声明为virtual的，
// 那么对于其派生类B而言，f总是能够被重写的。
// 有的时候并不想fun在B类型派生类中被重写，C++98没有方法对此进行限制。
// 对于Java这种所有类型派生于单一元类型（Object）的语言来说，
// 这种问题早就出现了，因此Java使用final关键字来阻止函数继续重写。
// final的作用是使派生类不可覆盖它所修饰的虚函数，C++11也采用了类似的做法：
struct Object {
  virtual void fun() = 0;
  virtual void fun1() final;
};
struct Base : public Object {
  void fun() final;  // 声明为final
  // void fun1();  // 报错
  void fun1(int);  // 重定义，不会报错
};
struct Derived : public Base {
  // void fun();  // 报错
};

// final都是用于派生类的，基类中的虚函数也可以使用final，比如fun1，
// 不过这样将使用该虚函数无法被重载，也就失去了虚函数的意义。
// 如果不想成员函数被重载，程序员可以直接将该成员函数定义为非虚的。
// 而final通常只在继承关系的中途终止派生类的重载中有意义。
// 从接口派生的角度而言，final可以在派生过程中任意地阻止一个接口的可重载性，
// 这给程序员带来了更大的控制力。
}  // namespace n2

namespace n3 {
// 在C++中重载还有一个特点，就是对于基类声明为virtual的函数，
// 之后的重载版本都不需要再声明该重载函数为virtual，
// 即使在派生类中声明了virtual，该关键字也是编译器可以忽略的。
// 书写便利同时却带来了阅读上的困难，无法看出一个函数是虚函数还是非虚函数。
// 虚函数会跨层，没有在父类中声明的接口有可能是祖先的虚函数接口，
// 这样在父类中也无法读到的信息，如果类的继承结构比较长或者比较复杂（多继承），
// 派生类的编写者会遇到信息分散、难以阅读的问题，编辑器的提示又不总是有效。
// 而自己是否在重载一个接口，以及是否有拼写错误等，都非常不容易检查。
// 在C++11中为了帮助程序员写继承结构复杂的类型，引入了虚函数描述符override，
// 如果派生类在虚函数声明时使用了override描述符，
// 那么该函数必须重载其基类中的同名函数，否则代码将无法通过编译。
struct Base {
  virtual void Turing() = 0;
  virtual void Dijkstra() = 0;
  virtual void VNeumann(int g) = 0;
  virtual void DKnuth() const;
  void Print();
};

struct DerivedMid : public Base {
  void VNeumann(double g);
  // 接口被隔离了，多一个版本的VNeumann函数
};

struct DerivedTop : public DerivedMid {
  void Turing() override;
  // 如果没有override修饰符，可能意识不到犯了这么多错误：
  // void Dikjstra() override;          // 拼写错误，并非重载
  // void VNeumann(double g) override;  // 参数不一致，并非重载
  // void DKnuth() override;            // 常量性不一致，并非重载
  // void Print() override;             // 非虚函数重载
  void Dikjstra();
  void VNeumann(double g);
  void DKnuth();
  void Print();
};
}  // namespace n3

namespace n4 {
// final/override也可以定义为正常变量名，
// 只有在其出现在函数后时才是能够控制继承/派生的关键字。
// 这样很多含有final/override变量或者函数名的C++98代码仍然不受影响。
// 但出于安全考虑，建议在C++11应该尽可能地避免这样的变量或宏。
int final;
int override;
}  // namespace n4

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 0]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}