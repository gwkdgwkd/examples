#include <iostream>

namespace n1 {
// C++11扩展的friend语法:
// 1.声明一个类为另外一个类的友元时，不再需要使用class关键字；
// 2.声明友元类时，甚至可以使用类的别名。

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
}  // namespace n1

namespace n2 {
// 1.普通类A的普通类B友元（一对一友好关系）：
// 无需前置声明class B，当class B第一次出现在friend声明中时，该类名字被隐式地认为可见。
class A {
 public:
  A(int a) : m_a(a) {}

 private:
  friend class B;
  int m_a;
};
class B {
 public:
  void access(A& obj) { std::cout << obj.m_a << std::endl; }

 private:
  int m_b;
};

void testN2() {
  A a(6);
  B b;
  b.access(a);  // 6
}
}  // namespace n2

namespace n3 {
// 2.如果普通类A的友元是一个实例化的特定模板，那么这个具体实例化的类在被声明为友元时，要带具体的类型参数。
// 正如使用vector一样，如vector<int>，类比等价于friend class B<int>。
// 但是，与vector<int>相比，编译器（毕竟没有那么智能）在读取class B<int>时会觉得很怪异，
// 不知道这个<int>是什么东西，因此会报错。
// 为了让编译器认识它，知道它是个实例化的模板，必须在class声明之前先提前告知编译器class B原本是个模板。
// 而friend声明友元时才表明这只是某一个具体模板的实例化。
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
  void access(A& obj) { std::cout << obj.m_a << std::endl; }
};

void testN3() {
  A a(8);
  B<int> b1;
  b1.access(a);  // 8

  B<float> b2;
  // b2.access(a);  // 报错
}
}  // namespace n3

namespace n4 {
// 3.普通类A的模板类B友元（一对多友好关系）：
// 普通类class A的友元是模板class B，而不是某一个具体实例化的class B<>，
// 即只要是这个模板实例化出来的B都是A的友元。
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
  void access(A& obj) { std::cout << obj.m_a << std::endl; }
};

void testN4() {
  A a(5);
  B<int> b1;
  b1.access(a);  // 5

  B<float> b2;
  b2.access(a);  // 5

  B<int> b3;
  b3.access(a);  // 5
}
}  // namespace n4

namespace n5 {
// 4.实例化实例化模板类A的实例化模板类B友元（一对一友好关系）：
// 与第2类同理，模板类A的友元类B也是一个模板，为了让编译器识别，必须得提前声明其是模板。
// 另外，class A和class B使用相同模板类型参数，
// 因此在使用相同类型实例化后得到的实例类是对应类型的一对一友好关系。
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
  void access(A<U>& obj) { std::cout << obj.m_a << std::endl; }
};

void testN5() {
  A<int> a1(3);
  B<int> b1;
  b1.access(a1);  // 3

  A<float> a2(2.56);
  B<float> b2;
  b2.access(a2);  // 2.56
  // b2.access(a1);  // 报错
}
}  // namespace n5

namespace n6 {
// 5.模板类A的实例化模板类B友元（多对一友好关系）：
// 与第2类同理，这里模板类A的友元类B也是一个模板，为了让编译器识别，必须得提前声明其是模板。
// B是一个特定的实例化的模板类，它是任意模板A实例化后的友元。
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
  void access(A<U>& obj) { std::cout << obj.m_a << std::endl; }
};

void testN6() {
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
}  // namespace n6

namespace n7 {
// 6.任意实例化模板类A的模板类B友元（多对多友好关系）：
// 任意一个实例化的A对任意实例化的B都是友好关系。
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
  void access(A<U>& obj) { std::cout << obj.m_a << std::endl; }
};

void testN7() {
  A<int> a1(5);
  A<int> a2(6);
  B<int> b1;
  B<int> b2;

  b1.access(a1);  // 5
  b1.access(a2);  // 6
  b2.access(a1);  // 5
  b2.access(a2);  // 6
}
}  // namespace n7

// 存在模板类A的某一特定实例化对模板类B的任意实例化都是友好关系的场景吗？
// 回答：不存在。
// 因为实例化A时，A必然已经定义过了，对于一个定义过了的模板，再去添加模板的友元，这是不符合逻辑的。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 5]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n2::testN2();
      break;
    case 1:
      n3::testN3();
      break;
    case 2:
      n4::testN4();
      break;
    case 3:
      n5::testN5();
      break;
    case 4:
      n6::testN6();
      break;
    case 5:
      n7::testN7();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}