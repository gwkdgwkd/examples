#include <iostream>

using namespace std;

// 1. 函数、类、类的成员函数作为类模板的友元
// 类模板实例化时，除了类型参数被替换外，其他所有内容都原样保留，因此任何从Tmp1实例化得到的类都包含上面三条友元声明，因而也都会把Func1、类A和B::Func当作友元。
void Func1() {}
class A1 {};
class B1 {
 public:
  void Func() {}
};
template <class T>
class Tmpl1 {
  friend void Func1();
  friend class A1;
  friend void B1::Func();
};

// 2. 函数模板作为类模板的友元
template <class T1, class T2>
class Pair {
 private:
  T1 key;
  T2 value;

 public:
  Pair(T1 k, T2 v) : key(k), value(v){};
  bool operator<(const Pair<T1, T2>& p) const;
  template <class T3, class T4>
  friend ostream& operator<<(ostream& o, const Pair<T3, T4>& p);
};
template <class T1, class T2>
bool Pair<T1, T2>::operator<(const Pair<T1, T2>& p) const {
  return key < p.key;
}
template <class T1, class T2>
ostream& operator<<(ostream& o, const Pair<T1, T2>& p) {
  o << "(" << p.key << "," << p.value << ")";
  return o;
}

// 3. 函数模板作为类的友元。类还可以将类模板或类模板的成员函数声明为友元。
class A3 {
  int v;

 public:
  A3(int n) : v(n) {}
  template <class T>
  friend void Print(const T& p);
};
template <class T>
void Print(const T& p) {
  cout << p.v << endl;
}

// 4. 类模板作为类模板的友元
template <class T>
class A4 {
 public:
  void Func(const T& p) { cout << p.v << endl; }
};
template <class T>
class B4 {
 private:
  T v;

 public:
  B4(T n) : v(n) {}
  template <class T2>
  friend class A4;  // 把类模板A声明为友元
};

int main() {
  Tmpl1<int> i;
  Tmpl1<double> f;

  Pair<string, int> student("Tom", 29);
  Pair<int, double> obj(12, 3.14);
  cout << student << " " << obj << endl;  // (Tom,29) (12,3.14)

  A3 a(4);
  Print(a);  // 4

  B4<int> b(5);
  A4<B4<int>> a1;  // 用B<int>替换A模板中的T
  a1.Func(b);      // 5

  return 0;
}