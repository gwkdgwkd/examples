#include <iostream>

using namespace std;

class Poly;
typedef Poly P;

class LiLei {
  friend class Poly;  // C++98通过, C++11通过
};

class Jim {
  friend Poly;  // C++98失败, C++11通过
};

class HanMeiMei {
  friend P;  // C++98失败, C++11通过
};

// C++98也能编译过，为什么？编译器比较新？

// C++11扩展的friend语法:
// 1 声明一个类为另外一个类的友元时，不再需要使用class关键字
// 2 声明友元类时，甚至可以使用类的别名。

// 1）普通类A的普通类B友元（一对一友好关系）：
// 无需前置声明class B，当class B第一次出现在friend声明中时，该类名字被隐式地认为可见。
class A1 {
  friend class B1;
  int m_a;
};
class B1 {
 public:
  void accessA(A1& objA) { cout << objA.m_a << endl; }

 private:
  int m_b;
};

// 2）如果普通类A的友元是一个实例化的特定模板，那么这个具体实例化的类在被声明为友元时，要带具体的类型参数。
// 正如使用vector一样，如vector<int>，类比等价于friend class B<int>。
// 但是，与vector<int>相比，编译器（毕竟没有那么智能）在读取class B<int>时会觉得很怪异，不知道这个<int>是什么东西，因此会报错。
// 为了让编译器认识它，知道它是个实例化的模板，必须在class声明之前先提前告知编译器class B原本是个模板。
// 而friend声明友元时才表明这只是某一个具体模板的实例化。
template <typename>
class B2;
class A2 {
  friend class B2<int>;
  int m_a;
};
template <typename T>
class B2 {
 public:
  void accessA(A2& objA) { cout << objA.m_a; }
};

// 3）普通类A的模板类B友元（一对多友好关系）：
// 这里普通类class A的友元是模板class B，而不是某一个具体实例化的class B<>，即只要是这个模板实例化出来的B都是A的友元。
class A3 {
  template <typename T>
  friend class B3;
  int m_a;
};
template <typename T>
class B3 {
 public:
  void accessA(A3& objA) { cout << objA.m_a; }
};

// 4）实例化实例化模板类A的实例化模板类B友元（一对一友好关系）：
// 与第2类同理，模板类A的友元类B也是一个模板，为了让编译器识别，必须得提前声明其是模板。
// 另外，class A和class B使用相同模板类型参数，因此在使用相同类型实例化后得到的实例类是对应类型的一对一友好关系。
template <typename>
class B4;
template <typename T>
class A4 {
  friend class B4<T>;
  T m_a;
};
template <typename U>
class B4 {
 public:
  void accessA(A4<U>& objA) { cout << objA.m_a; }
};

// 5）模板类A的实例化模板类B友元（多对一友好关系）：
// 与第2类同理，这里模板类A的友元类B也是一个模板，为了让编译器识别，必须得提前声明其是模板。
// B是一个特定的实例化的模板类，它是任意模板A实例化后的友元。
template <typename>
class B5;
template <typename T>
class A5 {
  friend class B5<int>;
  T m_a;
};
template <typename U>
class B5 {
 public:
  void accessA(A5<U>& objA) { cout << objA.m_a; }
};

// 6）任意实例化模板类A的模板类B友元（多对多友好关系）：
// 任意一个实例化的A对任意实例化的B都是友好关系。
template <typename T>
class A6 {
  template <typename X>
  friend class B6;
  T m_a;
};
template <typename U>
class B6 {
 public:
  void accessA(A6<U>& objA) { cout << objA.m_a; }
};

// 存在模板类A的某一特定实例化对模板类B的任意实例化都是友好关系的场景吗？
// 回答：不存在。
// 因为实例化A时，A必然已经定义过了，对于一个定义过了的模板，再去添加模板的友元，这是不符合逻辑的。

int main() { return 0; }