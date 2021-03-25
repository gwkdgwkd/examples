#include <iostream>

using namespace std;

// 类模板和类模板之间、类模板和类之间可以互相继承。它们之间的派生关系有以下四种情况。
// 1. 类模板从类模板派生
template <class T1, class T2>
class A1 {
  T1 v1;
  T2 v2;
};
template <class T1, class T2>
class B1 : public A1<T2, T1> {
  T1 v3;
  T2 v4;
};
template <class T>
class C1 : public B1<T, T> {
  T v5;
};

// 2. 类模板从模板类派生
template <class T1, class T2>
class A2 {
  T1 v1;
  T2 v2;
};
template <class T>
class B2 : public A2<int, double> {
  T v;
};

// 3. 类模板从普通类派生
class A3 {
  int v1;
};
template <class T>
class B3 : public A3 {
  T v;
};

// 4. 普通类从模板类派生
template <class T>
class A4 {
  T v1;
  int n;
};
class B4 : public A4<int> {
  double v;
};

int main() {
  B1<int, double> obj1;
  C1<int> obj2;

  B2<char> obj3;

  B3<char> obj4;

  B4 obj5;

  return 0;
}