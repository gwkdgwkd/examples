#include <iostream>

using namespace std;

// 在C++中，一个类中可以有public、protected、private三种属性的成员，通过对象可以访问public成员，只有本类中的函数可以访问本类的private成员。
// 借助友元（friend），可以使得其他类中的成员函数以及全局范围内的函数访问当前类的private成员。
// 在当前类以外定义的、不属于当前类的函数也可以在类中声明，但要在前面加friend关键字，这样就构成了友元函数。友元函数可以是不属于任何类的非成员函数，也可以是其他类的成员函数。
// 友元函数可以访问当前类中的所有成员，包括public、protected、private属性的。
// 成员函数在调用时会隐式地增加this指针，指向调用它的对象，从而使用该对象的成员；而show()是非成员函数，没有this指针，编译器不知道使用哪个对象的成员，
// 要想明确这一点，就必须通过参数传递对象（可以直接传递对象，也可以传递对象指针或对象引用），并在访问成员时指明对象。
// friend函数不仅可以是全局函数（非成员函数），还可以是另外一个类的成员函数。
// 一般情况下，类必须在正式声明之后才能使用；但是某些情况下，只要做好提前声明，也可以先使用。类的提前声明的使用范围是有限的，只有在正式声明一个类以后才能用它去创建对象。
// 因为创建对象时要为对象分配内存，在正式声明类之前，编译器无法确定应该为对象分配多大的内存。编译器只有在“见到”类的正式声明后（其实是见到成员变量），才能确定应该为对象
// 预留多大的内存。在对一个类作了提前声明后，可以用该类的名字去定义指向该类型对象的指针变量或引用变量，因为指针和引用本身的大小是固定的，与它所指向的数据的大小无关。
// 一个函数可以被多个类声明为友元函数，这样就可以访问多个类中的private成员。
// 不仅可以将一个函数声明为一个类的“朋友”，还可以将整个类声明为另一个类的“朋友”，这就是友元类。友元类中的所有成员函数都是另外一个类的友元函数。
// 关于友元，有两点需要说明：
//  友元的关系是单向的而不是双向的。如果声明了类B是类A的友元类，不等于类A是类B的友元类，类A中的成员函数不能访问类B中的private成员。
//  友元的关系不能传递。如果类B是类A的友元类，类C是类B的友元类，不等于类C是类A的友元类。
// 除非有必要，一般不建议把整个类声明为友元类，而只将某些成员函数声明为友元函数，这样更安全一些。

class Address;  // 如果不提前声明，编译器会报错
class Student {
 public:
  Student(char *name, int age, float score);

 public:
  friend void show(Student *pstu);
  void show1(Address *addr);
  void show2(Address *addr);

 private:
  char *m_name;
  int m_age;
  float m_score;
};
Student::Student(char *name, int age, float score)
    : m_name(name), m_age(age), m_score(score) {}

void show(Student *pstu) {
  cout << pstu->m_name << "的年龄是 " << pstu->m_age << "，成绩是 "
       << pstu->m_score << endl;
}
// 友元函数不同于类的成员函数，在友元函数中不能直接访问类的成员，必须要借助对象。下面的写法是错误的：
// void show() {
//   cout << m_name << "的年龄是 " << m_age << "，成绩是 " << m_score << endl;
// }

class Address {
 private:
  char *m_province;
  char *m_city;
  char *m_district;

 public:
  Address(char *province, char *city, char *district);
  friend void Student::show1(Address *addr);
  // 有的编译器也可以不写class关键字，不过为了增强兼容性还是建议写上。
  friend class Student;
};
// 程序将Student类的声明和实现分开了，而将Address类的声明放在了中间，这是因为编译器从上到下编译代码，show()函数体中用到了Address的成员province、city、district，
// 如果提前不知道Address的具体声明内容，就不能确定Address是否拥有该成员（类的声明中指明了类有哪些成员）。
void Student::show1(Address *addr) {
  cout << m_name << "的年龄是 " << m_age << "，成绩是 " << m_score << endl;
  cout << "家庭住址：" << addr->m_province << "省" << addr->m_city << "市"
       << addr->m_district << "区" << endl;
}
void Student::show2(Address *addr) {
  cout << m_name << "的年龄是 " << m_age << "，成绩是 " << m_score << endl;
  cout << "家庭住址：" << addr->m_province << "省" << addr->m_city << "市"
       << addr->m_district << "区" << endl;
}

Address::Address(char *province, char *city, char *district) {
  m_province = province;
  m_city = city;
  m_district = district;
}

int main() {
  Student stu((char *)"小明", 15, 90.6);
  show(&stu);
  Student *pstu = new Student((char *)"李磊", 16, 80.5);
  show(pstu);

  Address addr((char *)"陕西", (char *)"西安", (char *)"雁塔");
  stu.show1(&addr);
  Address *paddr = new Address((char *)"河北", (char *)"衡水", (char *)"桃城");
  pstu->show1(paddr);

  stu.show2(&addr);
  pstu->show2(paddr);

  return 0;
}