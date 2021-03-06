#include <iostream>

using namespace std;

// C++继承中重载、重写、重定义的区别：
// 一、重载overload：是函数名相同，参数列表不同 重载只是在类的内部存在。但是不能靠返回类型来判断。
// 二、重写override：也叫做覆盖。子类重新定义父类中有相同名称和参数的虚函数。函数特征相同。但是具体实现不同，主要是在继承关系中出现的 。
// 重写需要注意：
// 1 被重写的函数不能是static的。必须是virtual的
// 2 重写函数必须有相同的类型，名称和参数列表
// 3 重写函数的访问修饰符可以不同。尽管virtual是private的，派生类中重写改写为public,protected也是可以的
// 三、重定义 (redefining)也叫做隐藏:
// 子类重新定义父类中有相同名称的非虚函数(参数列表可以不同) 。
// 如果一个类，存在和父类相同的函数，那么，这个类将会覆盖其父类的方法，除非你在调用的时候，强制转换为父类类型，否则试图对子类和父类做类似重载的调用是不能成功的。

class Base {
 private:
  virtual void display() { cout << "Base display()" << endl; }
  void say() { cout << "Base say()" << endl; }

 public:
  void exec() {
    display();
    say();
  }
  // overload，两个f1函数在Base类的内部被重载
  void f1(string a) { cout << "Base f1(string)" << endl; }
  void f1(int a) { cout << "Base f1(int)" << endl; }
};

class DeriveA : public Base {
 public:
  // override，基类中display为虚函数，故此处为重写
  void display() { cout << "DeriveA display()" << endl; }
  // redefining，f1函数在Base类中不为虚函数，故此处为重定义
  void f1(int a, int b) { cout << "DeriveA f1(int,int)" << endl; }
  // redefining，同上
  void say() { cout << "DeriveA say()" << endl; }
};

class DeriveB : public Base {
 public:
  // redefining，重定义
  void f1(int a) { cout << "DeriveB f1(int)" << endl; }
};

// 综上所述，总结如下：
// 1 成员函数重载特征：
//    a 相同的范围（在同一个类中）
//    b 函数名字相同
//    c 参数不同
//    d virtual关键字可有可无
// 2 重写（覆盖)是指派生类函数覆盖基类函数，特征是：
//    a 不同的范围，分别位于基类和派生类中
//    b 函数的名字相同
//    c 参数相同
//    d 基类函数必须有virtual关键字
// 3 重定义(隐藏)是指派生类的函数屏蔽了与其同名的基类函数，规则如下：
//    a 如果派生类的函数和基类的函数同名，但是参数不同，此时，不管有无virtual，基类的函数被隐藏。
//    b 如果派生类的函数与基类的函数同名，并且参数也相同，但是基类函数没有vitual关键字，此时，基类的函数被隐藏。

// 注意区分虚函数中的重载和重写：
// class A {
//  public:
//   virtual int fun() {}
// };
// class B : public A {
//   int fun(int a) {}  // 这是重载而不是重写
// };
// class B : public A {
//   int fun();  // 从A继承来的fun, 编译器会自己偷偷帮你加上
//   int fun(int a) {}  // 新的fun, 和前面的只是名字一样的重载函数, 不是虚函数
// };

// C++中实现多态:
// 一个类A中声明的虚函数fun在其派生类B中再次被定义，且B中的函数fun跟A中fun的原型一样（函数名、参数列表等一样），那么我们就称B重写（override）了A的fun函数。
// 对于任何B类型的变量，调用成员函数fun都是调用了B重写的版本。而如果同时有A的派生类C，却并没有重写A的fun函数，那么调用成员函数fun则会调用A中的版本。

// 在通常情况下，一旦在基类A中的成员函数fun被声明为virtual的，那么对于其派生类B而言，fun总是能够被重写的。
// 有的时候我们并不想fun在B类型派生类中被重写，那么，C++98没有方法对此进行限制。
// 对于Java这种所有类型派生于单一元类型（Object）的语言来说，这种问题早就出现了。因此Java语言使用了final关键字来阻止函数继续重写。
// final关键字的作用是使派生类不可覆盖它所修饰的虚函数。C++11也采用了类似的做法：
struct Object1 {
  virtual void fun() = 0;
  virtual void fun1() final;
};
struct Base1 : public Object1 {
  void fun() final;  // 声明为final
};
struct Derived1 : public Base1 {
  // void fun();  // 无法通过编译
};

// final关键字都是用于描述一个派生类的。那么基类中的虚函数是否可以使用final关键字呢(fun1)？可以，不过这样将使用该虚函数无法被重载，也就失去了虚函数的意义。
// 如果不想成员函数被重载，程序员可以直接将该成员函数定义为非虚的。而final通常只在继承关系的“中途”终止派生类的重载中有意义。从接口派生的角度而言，final可以在
// 派生过程中任意地阻止一个接口的可重载性，这就给面向对象的程序员带来了更大的控制力。

// 在C++中重载还有一个特点，就是对于基类声明为virtual的函数，之后的重载版本都不需要再声明该重载函数为virtual。即使在派生类中声明了virtual，该关键字也是编译
// 器可以忽略的。这带来了一些书写上的便利，却带来了一些阅读上的困难。无法从定义中看出一个函数是虚函数还是非虚函数。另外一点就是，在C++中有的虚函数会“跨层”，没有
// 在父类中声明的接口有可能是祖先的虚函数接口。这同样是在父类中无法读到的信息。这样一来，如果类的继承结构比较长（不断地派生）或者比较复杂（比如偶尔多重继承），派
// 生类的编写者会遇到信息分散、难以阅读的问题（虽然有时候编辑器会进行提示，不过编辑器不是总是那么有效）。而自己是否在重载一个接口，以及自己重载的接口的名字是否有
// 拼写错误等，都非常不容易检查。
// 在C++11中为了帮助程序员写继承结构复杂的类型，引入了虚函数描述符override，如果派生类在虚函数声明时使用了override描述符，那么该函数必须重载其基类中的同名函数，
// 否则代码将无法通过编译。
struct Base2 {
  virtual void Turing() = 0;
  virtual void Dijkstra() = 0;
  virtual void VNeumann(int g) = 0;
  virtual void DKnuth() const;
  void Print();
};

struct DerivedMid : public Base2 {
  // void VNeumann(double g);
  // 接口被隔离了，曾想多一个版本的VNeumann函数
};

struct DerivedTop : public DerivedMid {
  void Turing() override;
  // void Dikjstra() override;  // 无法通过编译，拼写错误，并非重载
  // void VNeumann(double g) override;  // 无法通过编译，参数不一致，并非重载
  // void DKnuth() override;  // 无法通过编译，常量性不一致，并非重载
  // void Print() override;  // 无法通过编译，非虚函数重载
  // 如果没有override修饰符，DerivedTop的作者可能在编译后都没有意识到自己犯了这么多错误。
  void Dikjstra();
  void VNeumann(double g);
  void DKnuth();
  void Print();
};

// final/override也可以定义为正常变量名，只有在其出现在函数后时才是能够控制继承/派生的关键字。
// 通过这样的设计，很多含有final/override变量或者函数名的C++98代码就能够被C++编译器编译通过了。
// 但出于安全考虑，建议在C++11代码中应该尽可能地避免这样的变量名称或将其定义在宏中，以防发生不必要的错误。
int final;
int override;

int main() {
  DeriveA a;
  Base *b = &a;
  b->exec();  // b里边的函数display被A类覆盖，但是say还是自己的。
  // DeriveA display() // display():version of DeriveA call(polymorphism)
  // Base say() // say():version of Base called(allways)
  a.exec();  // same result as last statement
  // DeriveA display()
  // Base say()
  a.say();  // DeriveA say()
  DeriveB c;
  c.f1(1);
  // DeriveB f1(int) // version of DeriveB called

  return 0;
}