#include <cstring>
#include <iostream>

// C++除了支持函数模板，还支持类模板（Class Template）。
// 函数模板中定义的类型参数可以用在函数声明和函数定义中，
// 类模板中定义的类型参数可以用在类声明和类实现中。
// 类模板的目的同样是将数据的类型参数化。
// 类模板和函数模板都是以template开头（class也行），后跟类型参数；
// 类型参数不能为空，多个类型参数用逗号隔开。
// 一但声明了类模板，就可以将类型参数用于类的成员函数和成员变量了。

namespace n1 {
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

// 在类外定义成员函数时，仍然需要带上模板头，
// 而且template后面的类型参数要和类声明时的一致：
template <typename T1, typename T2>
T2 Point<T1, T2>::getY() const {
  return m_y;
}
// 类名Point后面也要带上类型参数，只是不加typename关键字了：
template <typename T1, typename T2>
void Point<T1, T2>::setY(T2 y) {
  m_y = y;
}

void func() {
  // 使用类模板创建对象时，需要指明具体的数据类型：
  // 与函数模板不同的是，类模板在实例化时必须显式地指明数据类型，
  // 编译器不能根据给定的数据推演出数据类型：
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

  // 赋值号两边都要指明具体的数据类型，且要保持一致，下面的写法是错误的：
  // Point<float, float> *p = new Point<float, int>(10.6, 109);
  // Point<float, float> *p = new Point(10.6, 109);
}
}  // namespace n1

namespace n2 {
// 类模板中的静态成员
namespace test1 {
template <class T>
class A {
 private:
  // 类模板可以定义静态成员，实例化得到的所有类都包含同样的静态成员：
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
template <>
int A<double>::count = 0;

void func() {
  // A<int>和A<double>是两个不同的类，都有count变量：
  A<int> ia1, ia2;
  A<double> da;

  // A<int>的对象和A<double>的对象不会共享一份count：
  ia1.PrintCount();  // 2
  ia2.PrintCount();  // 2
  da.PrintCount();   // 1
}
}  // namespace test1

namespace test2 {
template <typename T>
class A {
 public:
  // 类模板的静态数据成员分为两种情况：
  // 1.不依赖模版类型参数：
  static int si;
  static char sc;
  // 2.依赖模版类型参数：
  static T st;
};
// 对于不依赖模版类型参数的静态成员，有两种初始化方式：
// 1.具化定义，给特定类型T的静态成员一个特定的值：
template <>
int A<int>::si = 1;
template <>
int A<float>::si = 2;
template <>
int A<char>::si = 3;
// 2.范化定义，对任意类型T的都有一份通用的值：
template <typename T>
char A<T>::sc = 'a';
// 两种初始化可以并存，对于特定类型T如果存在具化定义，则以具化定义为准：
template <>
char A<char>::sc = 'b';
// 注意：
// 1.对于特定类型T的具化定义不能在不同的（cpp）实现文件中重复；
// 2.范化定义可以在不同的（cpp）实现文件重复也可以赋予不同数值，
//   链接器负责选出唯一定义，选择哪一个与具体的编译顺序有关。
// 因此，范化定义可以放在头文件中，具化定义放在唯一的cpp文件中。

// 对于依赖模版类型参数的静态成员，只有一种初始化方式：
template <>
int A<int>::st = 1;
template <>
float A<float>::st = 1.2f;
template <>
char A<char>::st = 'c';
// 由于st的定义依赖模版参数T的类型，所以定义st时必须给出T的类型，
// 同样，对于特定类型T的具化定义不可在不同的（CPP）实现文件中重复。

void func() {
  A<int> ia;
  A<float> fa;
  A<char> ca;

  std::cout << "si: " << ia.si << ", sc:" << ia.sc << ", st:" << ia.st
            << std::endl;  // si: 1, sc:a, st:1
  std::cout << "si: " << fa.si << ", sc:" << fa.sc << ", st:" << fa.st
            << std::endl;  // si: 2, sc:a, st:1.2
  std::cout << "si: " << ca.si << ", sc:" << ca.sc << ", st:" << ca.st
            << std::endl;  // si: 3, sc:b, st:c
}
}  // namespace test2
}  // namespace n2

namespace n3 {
// 类模板的友元
namespace test1 {
// 函数、类、类的成员函数作为类模板的友元
class B {
 public:
  void Func1();
  void Func2();
};
template <class T>
class TC {
  // 类模板实例化时，除了类型参数被替换外，其他所有内容都原样保留，
  // 因此任何从实例化得到的类都包含这三条友元声明，
  // 因而也都会把Func、类A和B::Func当作友元：
  friend void Func();
  friend class A;
  friend void B::Func1();

 private:
  void show() { std::cout << typeid(T).name() << std::endl; }
};
TC<int> i;
TC<double> d;
void Func() {
  i.show();
  d.show();
}
class A {
 public:
  void Func1() {
    i.show();
    d.show();
  }
  void Func2() {
    i.show();
    d.show();
  }
};
void B::Func1() {
  i.show();
  d.show();
}
void B::Func2() {
  // 报错：
  // i.show();
  // d.show();
}

void func() {
  Func();
  // i
  // d

  A a;
  a.Func1();
  a.Func2();
  // i
  // d
  // i
  // d

  B b;
  b.Func1();
  // i
  // d
}
}  // namespace test1

namespace test2 {
// 函数模板作为类模板的友元
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

namespace test3 {
// 函数模板作为类的友元
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

namespace test4 {
// 类模板作为类模板的友元
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
}  // namespace n3

namespace n4 {
// 类模板和类模板之间、类模板和类之间可以互相继承，有四种情况：
namespace test1 {
// 1.类模板从类模板派生
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

namespace test2 {
// 2.类模板从模板类派生
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

namespace test3 {
// 3.类模板从普通类派生
class A {
  int v1;
};
template <class T>
class B : public A {
  T v;
};
void func() { B<char> b; }
}  // namespace test3

namespace test4 {
// 4.普通类从模板类派生
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
}  // namespace n4

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = atoi(argv[1]);
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::test1::func();
      break;
    case 2:
      n2::test2::func();
      break;
    case 3:
      n3::test1::func();
      break;
    case 4:
      n3::test2::func();
      break;
    case 5:
      n3::test3::func();
      break;
    case 6:
      n3::test4::func();
      break;
    case 7:
      n4::test1::func();
      break;
    case 8:
      n4::test2::func();
      break;
    case 9:
      n4::test3::func();
      break;
    case 10:
      n4::test4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}