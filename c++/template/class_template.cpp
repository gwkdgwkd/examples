#include <cstring>
#include <iostream>

namespace n1 {
// C++除了支持函数模板，还支持类模板（Class Template）。
// 函数模板中定义的类型参数可以用在函数声明和函数定义中，类模板中定义的类型参数可以用在类声明和类实现中。
// 类模板的目的同样是将数据的类型参数化。
// 类模板和函数模板都是以template开头（当然也可以使用class，目前来讲它们没有任何区别），后跟类型参数；
// 类型参数不能为空，多个类型参数用逗号隔开。
// 一但声明了类模板，就可以将类型参数用于类的成员函数和成员变量了。
// 换句话说，原来使用int、float、char等内置类型的地方，都可以用类型参数来代替。
// 模板头和类头是一个整体，可以换行，但是中间不能有分号：
template <typename T1, typename T2>
class Point {
 public:
  Point(T1 x, T2 y) : m_x(x), m_y(y) {}

 public:
  T1 getX() const { return m_x; }
  void setX(T1 x) { m_x = x; }
  T2 getY() const;
  void setY(T2 y);

 private:
  T1 m_x;
  T2 m_y;
};

// 在类外定义成员函数时，仍然需要带上模板头，而且template后面的类型参数要和类声明时的一致：
template <typename T1, typename T2>
T2 Point<T1, T2>::getY() const {
  return m_y;
}
// 除了template关键字后面要指明类型参数，类名Point后面也要带上类型参数，只是不加typename关键字了：
template <typename T1, typename T2>
void Point<T1, T2>::setY(T2 y) {
  m_y = y;
}

void testN1() {
  // 使用类模板创建对象时，需要指明具体的数据类型
  // 与函数模板不同的是，类模板在实例化时必须显式地指明数据类型，编译器不能根据给定的数据推演出数据类型。
  Point<int, int> p1(10, 20);
  std::cout << p1.getX() << "," << p1.getY() << std::endl;  // 10,20
  Point<int, float> p2(10, 15.5);
  std::cout << p2.getX() << "," << p2.getY() << std::endl;  // 10,15.5
  Point<float, char *> p3(12.4, "nihao");
  std::cout << p3.getX() << "," << p3.getY() << std::endl;  // 12.4,nihao

  // 除了对象变量，也可以使用对象指针的方式来实例化：
  Point<float, float> *p4 = new Point<float, float>(10.6, 109.3);
  p4->setX(7.7);
  p4->setY(8.8);
  std::cout << p4->getX() << "," << p4->getY() << std::endl;  // 7.7,8.8
  Point<char *, char *> *p5 = new Point<char *, char *>("hello", "world");
  std::cout << p5->getX() << "," << p5->getY() << std::endl;  // hello,world

  // 赋值号两边都要指明具体的数据类型，且要保持一致。下面的写法是错误的：
  // Point<float, float> *p = new Point<float, int>(10.6, 109);
  // Point<float, float> *p = new Point(10.6, 109);
}
}  // namespace n1

namespace n2 {  // 类模板中的静态成员
template <class T>
class A {
 private:
  // 类模板中可以定义静态成员，从该类模板实例化得到的所有类都包含同样的静态成员：
  static int count;

 public:
  A() { count++; }
  ~A() { count--; };
  A(A &) { count++; }
  static void PrintCount() { std::cout << count << std::endl; }
};
// 对静态成员变量在类外部加以声明是必需的：
template <>
int A<int>::count = 0;
template <>  // 这里不用写class T了？
int A<double>::count = 0;

void testN2() {
  // A<int>和A<double>是两个不同的类：
  A<int> ia;
  A<double> da;

  // 虽然它们都有静态成员变量count，但是显然，A<int>的对象和A<double>的对象不会共享一份count：
  ia.PrintCount();  // 1
  da.PrintCount();  // 1
}
}  // namespace n2

namespace n3 {     // 类模板的友元
namespace test1 {  // 函数、类、类的成员函数作为类模板的友元
void Func() {}
class A {};
class B {
 public:
  void Func() {}
};
template <class T>
class TC {
  // 类模板实例化时，除了类型参数被替换外，其他所有内容都原样保留，
  // 因此任何从实例化得到的类都包含这三条友元声明，因而也都会把Func、类A和B::Func当作友元：
  friend void Func();
  friend class A;
  friend void B::Func();
};
void func() {
  TC<int> i;
  TC<double> f;
}
}  // namespace test1

namespace test2 {  // 函数模板作为类模板的友元
template <class T1, class T2>
class Pair {
 private:
  T1 key;
  T2 value;

 public:
  Pair(T1 k, T2 v) : key(k), value(v){};
  bool operator<(const Pair<T1, T2> &p) const;
  template <class T3, class T4>
  friend std::ostream &operator<<(std::ostream &o, const Pair<T3, T4> &p);
};
template <class T1, class T2>
bool Pair<T1, T2>::operator<(const Pair<T1, T2> &p) const {
  return key < p.key;
}
template <class T1, class T2>
std::ostream &operator<<(std::ostream &o, const Pair<T1, T2> &p) {
  o << "(" << p.key << "," << p.value << ")";
  return o;
}
void func() {
  Pair<std::string, int> student("Tom", 29);
  Pair<int, double> obj(12, 3.14);
  std::cout << student << " " << obj << std::endl;  // (Tom,29) (12,3.14)
}
}  // namespace test2

namespace test3 {  // 类模板的成员函数声明为友元
class A {
  int v;

 public:
  A(int n) : v(n) {}
  template <class T>
  friend void Print(const T &p);
};
template <class T>
void Print(const T &p) {
  std::cout << p.v << std::endl;
}
void func() {
  A a(4);
  Print(a);  // 4
}
}  // namespace test3

namespace test4 {  // 类模板作为类模板的友元
template <class T>
class A {
 public:
  void Func(const T &p) { std::cout << p.v << std::endl; }
};
template <class T>
class B {
 private:
  T v;

 public:
  B(T n) : v(n) {}
  template <class T2>
  friend class A;  // 把类模板A声明为友元
};
void func() {
  B<int> b(5);
  A<B<int>> a;  // 用B<int>替换A模板中的T
  a.Func(b);    // 5
}
}  // namespace test4

void testN3() {
  test1::func();
  test2::func();
  test3::func();
  test4::func();
}
}  // namespace n3

namespace n4 {  // 类模板和类模板之间、类模板和类之间可以互相继承，有四种情况
namespace test1 {  // 类模板从类模板派生
template <class T1, class T2>
class A {
  T1 v1;
  T2 v2;
};
template <class T1, class T2>
class B : public A<T2, T1> {
  T1 v3;
  T2 v4;
};
template <class T>
class C : public B<T, T> {
  T v5;
};

void func() {
  B<int, double> b;
  C<int> c;
}
}  // namespace test1

namespace test2 {  // 类模板从模板类派生
template <class T1, class T2>
class A {
  T1 v1;
  T2 v2;
};
template <class T>
class B : public A<int, double> {
  T v;
};
void func() { B<char> b; }
}  // namespace test2

namespace test3 {  // 类模板从普通类派生
class A {
  int v1;
};
template <class T>
class B : public A {
  T v;
};
void func() { B<char> b; }
}  // namespace test3

namespace test4 {  // 普通类从模板类派生
template <class T>
class A {
  T v1;
  int n;
};
class B : public A<int> {
  double v;
};
void func() { B b; }
}  // namespace test4

void testN4() {
  test1::func();
  test2::func();
  test3::func();
  test4::func();
}
}  // namespace n4

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
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
    case 3:
      n4::testN4();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}