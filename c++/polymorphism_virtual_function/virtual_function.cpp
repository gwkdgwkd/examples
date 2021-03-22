#include <iostream>

using namespace std;

// 基类的指针也可以指向派生类对象，使用了派生类的数据和基类函数。这与期望不同，指向了派生类，就应该用派生类的方法，这才符合人们的思维习惯。
// 换句话说，通过基类指针只能访问派生类的成员变量，但是不能访问派生类的成员函数。
// 为了消除这种尴尬，让基类指针能够访问派生类的成员函数，C++增加了虚函数（Virtual Function）。使用虚函数非常简单，只需要在函数声明前面增加virtual关键字。
// 有了虚函数，基类指针指向基类对象时就使用基类的成员（包括成员函数和成员变量），指向派生类对象时就使用派生类的成员。
// 换句话说，基类指针可以按照基类的方式来做事，也可以按照派生类的方式来做事，它有多种形态，或者说有多种表现方式，我们将这种现象称为多态（Polymorphism）。
// 多态是面向对象编程的主要特征之一，C++中虚函数的唯一用处就是构成多态。
// C++提供多态的目的是：可以通过基类指针对所有派生类（包括直接派生和间接派生）的成员变量和成员函数进行“全方位”的访问，尤其是成员函数。如果没有多态，我们只能访问成员变量。
// 通过指针调用普通的成员函数时会根据指针的类型（通过哪个类定义的指针）来判断调用哪个类的成员函数，
// 这种说法并不适用于虚函数，虚函数是根据指针的指向来调用的，指针指向哪个类的对象就调用哪个类的虚函数。
// 引用在本质上是通过指针的方式实现的，既然借助指针可以实现多态，那么就有理由推断：借助引用也可以实现多态。
// 当基类的引用指代基类对象时，调用的是基类的成员，而指代派生类对象时，调用的是派生类的成员。
// 不过引用不像指针灵活，指针可以随时改变指向，而引用只能指代固定的对象，在多态性方面缺乏表现力，所以以后我们再谈及多态时一般是说指针。
// 多态在小项目中鲜有有用武之地。对于具有复杂继承关系的大中型程序，多态可以增加其灵活性，让代码更具有表现力。

class People {
 public:
  People(char *name, int age);
  void display1();          // 声明为普通函数
  virtual void display2();  // 声明为虚函数
 protected:
  char *m_name;
  int m_age;
};
People::People(char *name, int age) : m_name(name), m_age(age) {}
void People::display1() {
  cout << "People(normal):" << m_name << "今年" << m_age
       << "岁了，是个无业游民。" << endl;
}
void People::display2() {
  cout << "People(virtual):" << m_name << "今年" << m_age
       << "岁了，是个无业游民。" << endl;
}
class Teacher : public People {
 public:
  Teacher(char *name, int age, int salary);
  void display1();          // 声明为普通函数
  virtual void display2();  // 声明为虚函数
 private:
  int m_salary;
};
Teacher::Teacher(char *name, int age, int salary)
    : People(name, age), m_salary(salary) {}
void Teacher::display1() {
  cout << "Teacher(normal):" << m_name << "今年" << m_age
       << "岁了，是一名教师，每月有" << m_salary << "元的收入。" << endl;
}
void Teacher::display2() {
  cout << "Teacher(virtual):" << m_name << "今年" << m_age
       << "岁了，是一名教师，每月有" << m_salary << "元的收入。" << endl;
}

// C++虚函数对于多态具有决定性的作用，有虚函数才能构成多态。虚函数的注意事项:
//  1) 只需要在虚函数的声明处加上virtual关键字，函数定义处可以加也可以不加。
//  2) 为了方便，你可以只将基类中的函数声明为虚函数，这样所有派生类中具有遮蔽关系的同名函数都将自动成为虚函数。
//  3) 当在基类中定义了虚函数时，如果派生类没有定义新的函数来遮蔽此函数，那么将使用基类的虚函数。
//  4) 只有派生类的虚函数覆盖基类的虚函数（函数原型相同）才能构成多态（通过基类指针访问派生类函数）。例如基类虚函数的原型为virtual void func();派生类虚函数的原
//     型为virtual void func(int);，那么当基类指针p指向派生类对象时，语句p->func(100);将会出错，而语句p->func();将调用基类的函数。
//  5) 构造函数不能是虚函数。对于基类的构造函数，它仅仅是在派生类构造函数中被调用，这种机制不同于继承。派生类不继承基类的构造函数，将构造函数声明为虚函数没有什么意义。
//  6) 析构函数可以声明为虚函数，而且有时候必须要声明为虚函数。
// 站在“学院派”的角度讲，封装、继承和多态是面向对象的三大特征。
// 构成多态的条件：
//  必须存在继承关系；
//  继承关系中必须有同名的虚函数，并且它们是覆盖关系（函数原型相同）。
//  存在基类的指针，通过该指针调用虚函数。
// 什么时候声明虚函数?
//  首先看成员函数所在的类是否会作为基类。
//  然后看成员函数在类的继承后有无可能被更改功能，如果希望更改其功能的，一般应该将它声明为虚函数。
//  如果成员函数在类被继承后功能不需修改，或派生类用不到该函数，则不要把它声明为虚函数。
class Base {
 public:
  virtual void func();
  virtual void func(int);
};
void Base::func() { cout << "void Base::func()" << endl; }
void Base::func(int n) { cout << "void Base::func(int)" << endl; }
class Derived : public Base {
 public:
  void func();  // 基类中将func声明为虚函数，派生类中的func就会自动成为虚函数。
  void func(char *);
};
void Derived::func() { cout << "void Derived::func()" << endl; }
void Derived::func(char *str) { cout << "void Derived::func(char *)" << endl; }

int main() {
  People *p = new People("王志刚", 23);
  p->display1();  // 基类指针指向基类对象，普通函数使用基类的数据调用基类的函数
  p->display2();  // 基类指针指向基类对象，虚函数使用基类的数据调用基类的函数
  p = new Teacher("赵宏佳", 45, 8200);
  p->display1();  // 基类指针指向派生类对象，普通函数使用派生类的数据调用基类的函数
  p->display2();  // 基类指针指向派生类对象，虚函数使用派生类的数据调用派生类的函数
  // People(normal):王志刚今年23岁了，是个无业游民。
  // People(virtual):王志刚今年23岁了，是个无业游民。
  // People(normal):赵宏佳今年45岁了，是个无业游民。
  // Teacher(virtual):赵宏佳今年45岁了，是一名教师，每月有8200元的收入。

  People p1("王志", 23);
  Teacher t1("赵宏", 45, 8200);
  People &rp = p1;
  People &rt = t1;
  rp.display1();  // 基类引用指向基类对象，普通函数使用基类的数据调用基类的函数
  rp.display2();  // 基类引用指向基类对象，虚函数使用基类的数据调用基类的函数
  rt.display1();  // 基类引用指向派生类对象，普通函数使用派生类的数据调用基类的函数
  rt.display2();  // 基类引用指向派生类对象，虚函数使用派生类的数据调用派生类的函数
  // People(normal):王志今年23岁了，是个无业游民。
  // People(virtual):王志今年23岁了，是个无业游民。
  // People(normal):赵宏今年45岁了，是个无业游民。
  // Teacher(virtual):赵宏今年45岁了，是一名教师，每月有8200元的收入。

  People p2("张三", 23);
  Teacher t2("李四", 45, 8200);
  People p3 = t2;
  p2.display1();  // 基类对象用基类对象赋值，普通函数使用基类数据调用基类的函数
  p2.display2();  // 基类对象用基类对象赋值，虚函数使用基类数据调用基类的函数
  p3.display1();  // 基类对象用派生类对象赋值，普通函数使用派生类对象调用基类的函数
  p3.display2();  // 基类对象用派生类对象赋值，虚函数使用派生类对象调用基类的函数
                  // People(normal):张三今年23岁了，是个无业游民。
  // People(virtual):张三今年23岁了，是个无业游民。
  // People(normal):李四今年45岁了，是个无业游民。
  // People(virtual):李四今年45岁了，是个无业游民。

  Base *pp = new Derived();  // p是基类的指针，但是指向了派生类的对象。
  pp->func();  // 调用的是派生类的虚函数，构成了多态。
  pp->func(10);  // 调用的是基类的虚函数，因为派生类中没有函数覆盖它。
  // 通过基类的指针只能访问从基类继承过去的成员，不能访问派生类新增的成员。下面的语句编译失败。
  // pp->func("http://c.biancheng.net");  // compile error
  // void Derived::func()
  // void Base::func(int)

  return 0;
}