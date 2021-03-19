#include <iostream>

using namespace std;

// 在类中，如果你不希望某些数据被修改，可以使用const关键字加以限定。const可以用来修饰成员变量和成员函数。
// const成员变量的用法和普通const变量的用法相似，只需要在声明时加上const关键字。初始化const成员变量只有一种方法，就是通过构造函数的初始化列表。

// const成员函数可以使用类中的所有成员变量，但是不能修改它们的值，这种措施主要还是为了保护数据而设置的。const成员函数也称为常成员函数。
// 常成员函数需要在声明和定义的时候在函数头部的结尾加上const关键字。必须在成员函数的声明和定义处同时加上const关键字。
// char *getname() const和char *getname()是两个不同的函数原型，如果只在一个地方加const会导致声明和定义处的函数原型冲突。
// 区分一下const的位置：
//  函数开头的const用来修饰函数的返回值，表示返回值是const类型，也就是不能被修改，例如const char * getname()。
//  函数头部的结尾加上const表示常成员函数，这种函数只能读取成员变量的值，而不能修改成员变量的值，例如char * getname() const。

// 在C++中，const也可以用来修饰对象，称为常对象。
// 一旦将对象定义为常对象之后，不管是哪种形式，该对象就只能访问被const修饰的成员了（包括const成员变量和const成员函数），因为非const成员可能会修改对
// 象的数据（编译器也会这样假设），C++禁止这样做。

class Student {
 public:
  Student(char *name, int age, float score);
  void show();
  char *getname() const;
  int getage() const;
  float getscore() const;

 private:
  char *m_name;
  int m_age;
  float m_score;
};
Student::Student(char *name, int age, float score)
    : m_name(name), m_age(age), m_score(score) {}
void Student::show() {
  cout << m_name << "的年龄是" << m_age << "，成绩是" << m_score << endl;
}
// getname()、getage()、getscore()三个函数的功能都很简单，仅仅是为了获取成员变量的值，没有任何修改成员变量的企图，
// 所以我们加了const限制，这是一种保险的做法，同时也使得语义更加明显。
char *Student::getname() const { return m_name; }
int Student::getage() const { return m_age; }
float Student::getscore() const { return m_score; }

int main() {
  // 定义常对象的语法和定义常量的语法类似：
  const Student s1((char *)"张三", 18, 10.5);
  Student const s2((char *)"李四", 17, 10.8);
  // 也可以定义const指针：
  const Student *p1 = new Student((char *)"张三", 18, 10.5);
  Student const *p2 = new Student((char *)"张三", 18, 10.5);

  const Student stu((char *)"小明", 15, 90.6);
  // stu.show();  // error
  cout << stu.getname() << "的年龄是" << stu.getage() << "，成绩是"
       << stu.getscore() << endl;
  const Student *pstu = new Student((char *)"李磊", 16, 80.5);
  // pstu -> show();  // error
  cout << pstu->getname() << "的年龄是" << pstu->getage() << "，成绩是"
       << pstu->getscore() << endl;

  return 0;
}