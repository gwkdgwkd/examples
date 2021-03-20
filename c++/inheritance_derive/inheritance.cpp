#include <iostream>

using namespace std;

// 继承（Inheritance）可以理解为一个类从另一个类获取成员变量和成员函数的过程。
// 在C++中，派生（Derive）和继承是一个概念，只是站的角度不同。继承是儿子接收父亲的产业，派生是父亲把产业传承给儿子。
// 被继承的类称为父类或基类，继承的类称为子类或派生类。“子类”和“父类”通常放在一起称呼，“基类”和“派生类”通常放在一起称呼。
// 派生类除了拥有基类的成员，还可以定义自己的新成员，以增强类的功能。
// 以下是两种典型的使用继承的场景：
//  1) 当你创建的新类与现有的类相似，只是多出若干成员变量或成员函数时，可以使用继承，这样不但会减少代码量，而且新类会拥有基类的所有功能。
//  2) 当需要创建多个类，拥有很多相似的成员变量或成员函数时，也可以使用继承。将这些类的共同成员提取出来，定义为基类，然后从基类继承，既可以节省代码，也方便后续修改成员。
// 继承的一般语法为：
// class 派生类名:［继承方式］ 基类名{
//     派生类新增加的成员
// };

// public、protected、private 三个关键字除了可以修饰类的成员，还可以指定继承方式。
// 继承方式包括public（公有的）、private（私有的）和 protected（受保护的），此项是可选的，如果不写，那么默认为private。
// 不同的继承方式会影响基类成员在派生类中的访问权限。
//  1) public继承方式
//     基类中所有public成员在派生类中为public属性；
//     基类中所有protected成员在派生类中为protected属性；
//     基类中所有private成员在派生类中不能使用。
//  2) protected继承方
//     基类中的所有public成员在派生类中为protected属性；
//     基类中的所有protected成员在派生类中为protected属性；
//     基类中的所有private成员在派生类中不能使用。
//  3) private继承方式
//     基类中的所有public成员在派生类中均为private属性；
//     基类中的所有protecte成员在派生类中均为private属性；
//     基类中的所有private成员在派生类中不能使用。
// 通过上面的分析可以发现：
//  1) 基类成员在派生类中的访问权限不得高于继承方式中指定的权限。例如，当继承方式为protected时，那么基类成员在派生类中的访问权限最高也为protected，
//     高于protected的会降级为protected，但低于protected不会升级。再如，当继承方式为public时，那么基类成员在派生类中的访问权限将保持不变。
//     也就是说，继承方式中的public、protected、private是用来指明基类成员在派生类中的最高访问权限的。
//  2) 不管继承方式如何，基类中的private成员在派生类中始终不能使用（不能在派生类的成员函数中访问或调用）。
//  3) 如果希望基类的成员能够被派生类继承并且毫无障碍地使用，那么这些成员只能声明为public或protected；只有那些不希望在派生类中使用的成员才声明为private。
//  4) 如果希望基类的成员既不向外暴露（不能通过对象访问），还能在派生类中使用，那么只能声明为protected。
// 注意，我们这里说的是基类的private成员不能在派生类中使用，并没有说基类的private成员不能被继承。
// 实际上，基类的private成员是能够被继承的，并且（成员变量）会占用派生类对象的内存，它只是在派生类中不可见，导致无法使用罢了。
// private成员的这种特性，能够很好的对派生类隐藏基类的实现，以体现面向对象的封装性。
// 继承方式/基类成员 	public成员 	protected成员 	private成员
// public继承 	        public 	    protected 	    不可见
// protected继承 	    protected 	protected 	    不可见
// private继承 	        private 	private 	    不可见
// 由于private和protected继承方式会改变基类成员在派生类中的访问权限，导致继承关系复杂，所以实际开发中我们一般使用public。
// 在派生类中访问基类private成员的唯一方法就是借助基类的非private成员函数，如果基类没有非private成员函数，那么该成员在派生类中将无法访问。
// 使用using关键字可以改变基类成员在派生类中的访问权限，例如将public改为private、将protected改为public。
// 注意：using只能改变基类中public和protected成员的访问权限，不能改变private成员的访问权限，因为基类中private成员在派生类中是不可见的，根本不能使用，
// 所以基类中的private成员在派生类中无论如何都不能访问。

// 如果派生类中的成员（包括成员变量和成员函数）和基类中的成员重名，那么就会遮蔽从基类继承过来的成员。
// 所谓遮蔽，就是在派生类中使用该成员（包括在定义派生类时使用，也包括通过派生类对象访问该成员）时，实际上使用的是派生类新增的成员，而不是从基类继承来的。
// 基类成员函数和派生类成员函数不构成重载:
//  基类成员和派生类成员的名字一样时会造成遮蔽，这句话对于成员变量很好理解，对于成员函数要引起注意，不管函数的参数如何，只要名字一样就会造成遮蔽。
//  换句话说，基类成员函数和派生类成员函数不会构成重载，如果派生类有同名函数，那么就会遮蔽基类中的所有同名函数，不管它们的参数是否一样。

class People {
 public:
  void setname(char *name);
  void setage(int age);
  void sethobby(char *hobby);
  char *gethobby();

 protected:
  char *m_name;
  int m_age;

 private:
  char *m_hobby;
};
void People::setname(char *name) { m_name = name; }
void People::setage(int age) { m_age = age; }
void People::sethobby(char *hobby) { m_hobby = hobby; }
char *People::gethobby() { return m_hobby; }

class Student : public People {
 public:
  void setscore(float score);

 protected:
  float m_score;
};
void Student::setscore(float score) { m_score = score; }

class Pupil : public Student {
 public:
  void setranking(int ranking);
  void display();

 private:
  int m_ranking;
};
void Pupil::setranking(int ranking) { m_ranking = ranking; }
void Pupil::display() {
  cout << m_name << "的年龄是" << m_age << "，考试成绩为" << m_score
       << "分，班级排名第" << m_ranking << "，TA喜欢" << gethobby() << "。"
       << endl;
}

class People1 {
 public:
  void show();

 protected:
  char *m_name;
  int m_age;
};
void People1::show() { cout << m_name << "的年龄是" << m_age << endl; }
class Student1 : public People1 {
 public:
  void learning();

 public:
  using People1::m_name;  // 将protected改为public
  using People1::m_age;   // 将protected改为public
  float m_score;

 private:
  using People1::show;  // 将public改为private
};
void Student1::learning() {
  cout << "我是" << m_name << "，今年" << m_age << "岁，这次考了" << m_score
       << "分！" << endl;
}

class People2 {
 public:
  void show();

 protected:
  char *m_name;
  int m_age;
};
void People2::show() {
  cout << "嗨，大家好，我叫" << m_name << "，今年" << m_age << "岁" << endl;
}
class Student2 : public People2 {
 public:
  Student2(char *name, int age, float score);

 public:
  void show();  // 遮蔽基类的show()
 private:
  float m_score;
};
Student2::Student2(char *name, int age, float score) {
  m_name = name;
  m_age = age;
  m_score = score;
}
void Student2::show() {
  cout << m_name << "的年龄是" << m_age << "，成绩是" << m_score << endl;
}

class Base {
 public:
  void func();
  void func(int);
};
void Base::func() { cout << "Base::func()" << endl; }
void Base::func(int a) { cout << "Base::func(int)" << endl; }
class Derived : public Base {
 public:
  void func(char *);
  void func(bool);
};
void Derived::func(char *str) { cout << "Derived::func(char *)" << endl; }
void Derived::func(bool is) { cout << "Derived::func(bool)" << endl; }

int main() {
  Pupil pup;
  pup.setname("小明");
  pup.setage(15);
  pup.setscore(92.5f);
  pup.setranking(4);
  pup.sethobby("乒乓球");
  pup.display();
  // 小明的年龄是15，考试成绩为92.5分，班级排名第4，TA喜欢乒乓球。

  Student1 stu;
  stu.m_name = "小明";
  stu.m_age = 16;
  stu.m_score = 99.5f;
  // stu.show();  // compile error
  stu.learning();
  // 我是小明，今年16岁，这次考了99.5分！

  Student2 stu2("小明", 16, 90.5);
  // 使用的是派生类新增的成员函数，而不是从基类继承的
  stu2.show();  // 小明的年龄是16，成绩是90.5
  // 使用的是从基类继承来的成员函数
  stu2.People2::show();  // 嗨，大家好，我叫小明，今年16岁

  Derived d;
  d.func("c.biancheng.net");
  d.func(true);
  // d.func();    // compile error
  // d.func(10);  // compile error
  d.Base::func();
  d.Base::func(100);

  return 0;
}