#include <iostream>

namespace n1 {
// 1.普通类A的普通类B友元（一对一友好关系）：
//   无需前置声明B，当B第一次出现在friend声明中时，该类名字被隐式地认为可见。
class A {
 public:
  A(int a) : m_a(a) {}

 private:
  friend class B;
  int m_a;
};
class B {
 public:
  void access(A &obj) { std::cout << obj.m_a << std::endl; }

 private:
  int m_b;
};

void func() {
  A a(6);
  B b;
  b.access(a);  // 6
}
}  // namespace n1

namespace n2 {
// 2.如果普通类A的友元是一个实例化的特定模板，
//   那么这个具体实例化的类在被声明为友元时，要带具体的类型参数。
//   正如使用vector一样，如vector<int>，类比等价于friend class B<int>。
//   但是，与vector<int>相比，编译器在读取class B<int>时会觉得很怪异，
//   不知道这个<int>是什么东西，因此会报错。
//   为了让编译器认识它，知道它是个实例化的模板，
//   必须在class声明之前先提前告知编译器class B原本是个模板。
//   而friend声明友元时才表明这只是某一个具体模板的实例化。
template <typename>
class B;
class A {
 public:
  A(int a) : m_a(a) {}

 private:
  friend class B<int>;
  int m_a;
};
template <typename T>
class B {
 public:
  void access(A &obj) { std::cout << obj.m_a << std::endl; }
};

void func() {
  A a(8);
  B<int> b1;
  b1.access(a);  // 8

  B<float> b2;
  // b2.access(a);  // 报错
}
}  // namespace n2

namespace n3 {
// 3.普通类A的模板类B友元（一对多友好关系）：
//   普通类A的友元是模板B，而不是某一个具体实例化的class B<>，
//   即只要是这个模板实例化出来的B都是A的友元。
class A {
 public:
  A(int a) : m_a(a) {}

 private:
  template <typename T>
  friend class B;
  int m_a;
};
template <typename T>
class B {
 public:
  void access(A &obj) { std::cout << obj.m_a << std::endl; }
};

void func() {
  A a(5);
  B<int> b1;
  b1.access(a);  // 5

  B<float> b2;
  b2.access(a);  // 5

  B<int> b3;
  b3.access(a);  // 5
}
}  // namespace n3

namespace n4 {
// 4.实例化模板类A的实例化模板类B友元（一对一友好关系）：
//   与第2类同理，模板类A的友元类B也是一个模板，
//   为了让编译器识别，必须得提前声明其是模板。
//   另外，class A和class B使用相同模板类型参数，
//   因此在使用相同类型实例化后得到的实例类是对应类型的一对一友好关系。
template <typename>
class B;
template <typename T>
class A {
 public:
  A(T a) : m_a(a) {}

 private:
  friend class B<T>;
  T m_a;
};
template <typename U>
class B {
 public:
  void access(A<U> &obj) { std::cout << obj.m_a << std::endl; }
};

void func() {
  A<int> a1(3);
  B<int> b1;
  b1.access(a1);  // 3

  A<float> a2(2.56);
  B<float> b2;
  b2.access(a2);  // 2.56
  // b2.access(a1);  // 报错
}
}  // namespace n4

namespace n5 {
// 5.模板类A的实例化模板类B友元（多对一友好关系）：
//   与第2类同理，这里模板类A的友元类B也是一个模板，
//   为了让编译器识别，必须得提前声明其是模板。
//   B是一个特定的实例化的模板类，它是任意模板A实例化后的友元。
template <typename>
class B;
template <typename T>
class A {
 public:
  A(T a) : m_a(a) {}

 private:
  friend class B<int>;
  T m_a;
};
template <typename U>
class B {
 public:
  void access(A<U> &obj) { std::cout << obj.m_a << std::endl; }
};

void func() {
  A<int> a1(2);
  A<int> a2(3);

  B<int> b;
  b.access(a1);  // 2
  b.access(a2);  // 3
  // 是这样多对一么？看描述不是这样的

  B<int> b2;
  b2.access(a1);  // 2
  // 这不是和下面多对多一样了么？
}
}  // namespace n5

namespace n6 {
// 6.任意实例化模板类A的模板类B友元（多对多友好关系）：
//   任意一个实例化的A对任意实例化的B都是友好关系。
template <typename T>
class A {
 public:
  A(T a) : m_a(a) {}

 private:
  template <typename X>
  friend class B;
  T m_a;
};
template <typename U>
class B {
 public:
  void access(A<U> &obj) { std::cout << obj.m_a << std::endl; }
};

void func() {
  A<int> a1(5);
  A<int> a2(6);
  B<int> b1;
  B<int> b2;

  b1.access(a1);  // 5
  b1.access(a2);  // 6
  b2.access(a1);  // 5
  b2.access(a2);  // 6
}
}  // namespace n6

// 存在模板类A的某一特定实例化对模板类B的任意实例化都是友好关系的场景吗？
// 回答：不存在。
// 因为实例化A时，A必然已经定义过了，对于一个定义过了的模板，
// 再去添加模板的友元，这是不符合逻辑的。

namespace n7 {
// 类模板的友元：函数、类、类的成员函数作为类模板的友元
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
}  // namespace n7

namespace n8 {
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
}  // namespace n8

namespace n9 {
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
}  // namespace n9

namespace n10 {
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
}  // namespace n10

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 6]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::func();
      break;
    case 2:
      n3::func();
      break;
    case 3:
      n4::func();
      break;
    case 4:
      n5::func();
      break;
    case 5:
      n6::func();
      break;
    case 6:
      n7::func();
      break;
    case 7:
      n8::func();
      break;
    case 8:
      n9::func();
      break;
    case 9:
      n10::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}