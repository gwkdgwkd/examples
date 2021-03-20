#include <iostream>

using namespace std;

// 基类的成员函数可以被继承，可以通过派生类的对象访问，但这仅仅指的是普通的成员函数，类的构造函数不能被继承。
// 构造函数不能被继承是有道理的，因为即使继承了，它的名字和派生类的名字也不一样，不能成为派生类的构造函数，当然更不能成为普通的成员函数。
// 对继承过来的成员变量的初始化工作也要由派生类的构造函数完成，但大部分基类都有private的成员变量，在派生类中无法访问，更不能使用派生类的构造函数来初始化。
// 这种矛盾在C++继承中是普遍存在的，解决这个问题的思路是：在派生类的构造函数中调用基类的构造函数。
// 可以将基类构造函数的调用放在参数初始化表前面或者后面，不管它们的顺序如何，派生类构造函数总是先调用基类构造函数再执行其他代码（包括参数初始化表以及函数体中的代码）。
// 构造函数的调用顺序:基类构造函数总是被优先调用，这说明创建派生类对象时，会先调用基类构造函数，再调用派生类构造函数。

// 派生类构造函数中只能调用直接基类的构造函数，不能调用间接基类的。
// A --> B --> C
// A类构造函数 --> B类构造函数 --> C类构造函数
// 构造函数的调用顺序是按照继承的层次自顶向下、从基类再到派生类的。
// C++这样规定是有道理的，因为我们在C中调用了B的构造函数，B又调用了A的构造函数，相当于C间接地（或者说隐式地）调用了A的构造函数，如果再在C中显式地调用A的构造函数，
// 那么A的构造函数就被调用了两次，相应地，初始化工作也做了两次，这不仅是多余的，还会浪费CPU时间以及内存，毫无益处，所以C++禁止在C中显式地调用A的构造函数。
// 通过派生类创建对象时必须要调用基类的构造函数，这是语法规定。换句话说，定义派生类构造函数时最好指明基类构造函数；
// 如果不指明，就调用基类的默认构造函数（不带参数的构造函数）；如果没有默认构造函数，那么编译失败。

// 和构造函数类似，析构函数也不能被继承。与构造函数不同的是，在派生类的析构函数中不用显式地调用基类的析构函数，因为每个类只有一个析构函数，编译器知道如何选择，无需程序员干涉。
// 析构函数的执行顺序和构造函数的执行顺序也刚好相反：
//  创建派生类对象时，构造函数的执行顺序和继承顺序相同，即先执行基类构造函数，再执行派生类构造函数。
//  而销毁派生类对象时，析构函数的执行顺序和继承顺序相反，即先执行派生类析构函数，再执行基类析构函数。

class People {
 protected:
  char *m_name;
  int m_age;

 public:
  // 如果将基类不带参数的构造函数删除，那么会发生编译错误，因为已经显式定义了构造函数，编译器不会再生成默认的构造函数。
  People();  // 基类默认构造函数
  People(char *, int);
};
People::People() : m_name("xxx"), m_age(0) {}
People::People(char *name, int age) : m_name(name), m_age(age) {}
class Student : public People {
 private:
  float m_score;

 public:
  Student();
  Student(char *name, int age, float score);
  void display();
};
Student::Student() : m_score(0.0) {}  // 派生类默认构造函数
Student::Student(char *name, int age, float score)
    : People(name, age), m_score(score) {}
// Student::Student(char *name, int age, float score): m_score(score), People(name, age){}  // 也可以
// Student::Student(char *name, int age, float score) {  // 就像这样
//   People(name, age);
//   m_score = score;
// }
// 不但可以是派生类构造函数参数列表中的参数，还可以是局部变量、常量等
// Student::Student(char *name, int age, float score): People("小明", 16), m_score(score){}
void Student::display() {
  cout << m_name << "的年龄是" << m_age << "，成绩是" << m_score << "。"
       << endl;
}

class A {
 public:
  A() { cout << "A constructor" << endl; }
  ~A() { cout << "A destructor" << endl; }
};
class B : public A {
 public:
  B() { cout << "B constructor" << endl; }
  ~B() { cout << "B destructor" << endl; }
};
class C : public B {
 public:
  C() { cout << "C constructor" << endl; }
  ~C() { cout << "C destructor" << endl; }
};

int main() {
  Student stu("小明", 16, 90.5);
  stu.display();  // 小明的年龄是16，成绩是90.5。

  Student stu1;
  stu1.display();  // xxx的年龄是0，成绩是0。

  C test;
  // A constructor
  // B constructor
  // C constructor
  // C destructor
  // B destructor
  // A destructor

  return 0;
}