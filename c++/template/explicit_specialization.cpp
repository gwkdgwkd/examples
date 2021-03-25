#include <iostream>

using namespace std;

// C++没有办法限制类型参数的范围，可以使用任意一种类型来实例化模板。但是模板中的语句（函数体或者类体）不一定就能适应所有的类型，可能会有个别的类型没有意义，或者会导致语法错误。
// template<class T> const T& Max(const T& a, const T& b){ return a > b ? a : b; }
// a>b这条语句，>能够用来比较int、float、char等基本类型数据的大小，但是却不能用来比较结构体变量、对象以及数组的大小，因为我们并没有针对结构体、类和数组重载>。
// 另外，该函数模板虽然可以用于指针，但比较的是地址大小，而不是指针指向的数据，所以也没有现实的意义。
// 模板是一种泛型技术，它能接受的类型是宽泛的、没有限制的，并且对这些类型使用的算法都是一样的（函数体或类体一样）。但是现在我们希望改变这种“游戏规则”，
// 让模板能够针对某种具体的类型使用不同的算法（函数体或类体不同），这在C++中是可以做到的，这种技术称为模板的显示具体化（Explicit Specialization）。
// 函数模板和类模板都可以显示具体化。

// 函数模板的显式具体化:
typedef struct {
  string name;
  int age;
  float score;
} STU;
// 函数模板
template <class T>
const T& Max(const T& a, const T& b);
// 函数模板的显示具体化（针对STU类型的显示具体化）
// template <>
// const STU& Max<STU>(const STU& a, const STU& b);
// Max<STU>中的STU是可选的，因为函数的形参已经表明，这是STU类型的一个具体化，编译器能够逆推出T的具体类型。简写后的函数声明为：
template <>
const STU& Max(const STU& a, const STU& b);
ostream& operator<<(ostream& out, const STU& stu);
template <class T>
const T& Max(const T& a, const T& b) {
  return a > b ? a : b;
}
template <>
const STU& Max<STU>(const STU& a, const STU& b) {
  return a.score > b.score ? a : b;
}
ostream& operator<<(ostream& out, const STU& stu) {
  out << stu.name << " , " << stu.age << " , " << stu.score;
  return out;
}
// 对于给定的函数名，可以有非模板函数、模板函数、显示具体化模板函数以及它们的重载版本，在调用函数时，显示具体化优先于常规模板，而非模板函数优先于显示具体化和常规模板。

// 除了函数模板，类模板也可以显示具体化，并且它们的语法是类似的。
template <class T1, class T2>
class Point {
 public:
  Point(T1 x, T2 y) : m_x(x), m_y(y) {}

 public:
  T1 getX() const { return m_x; }
  void setX(T1 x) { m_x = x; }
  T2 getY() const { return m_y; }
  void setY(T2 y) { m_y = y; }
  void display() const;

 private:
  T1 m_x;
  T2 m_y;
};
template <class T1, class T2>  // 这里要带上模板头
void Point<T1, T2>::display() const {
  cout << "x=" << m_x << ", y=" << m_y << endl;
}
// 类模板的显示具体化（针对字符串类型的显示具体化）
template <>
class Point<char*, char*> {
 public:
  Point(char* x, char* y) : m_x(x), m_y(y) {}

 public:
  char* getX() const { return m_x; }
  void setX(char* x) { m_x = x; }
  char* getY() const { return m_y; }
  void setY(char* y) { m_y = y; }
  void display() const;

 private:
  char* m_x;
  char* m_y;
};
// 这里不能带模板头template<>
void Point<char*, char*>::display() const {
  cout << "x=" << m_x << " | y=" << m_y << endl;
}
// 另外C++还允许只为一部分类型参数提供实参，这称为部分显式具体化。部分显式具体化只能用于类模板，不能用于函数模板。
// 类名后面之所以要列出所有的类型参数，是为了让编译器确认“到底是第几个类型参数被具体化了”，如果这样：
// template<typename T2> class Point<char*>
// 编译器就不知道char*代表的是第一个类型参数，还是第二个类型参数。
template <typename T2>
class Point<char*, T2> {
 public:
  Point(char* x, T2 y) : m_x(x), m_y(y) {}

 public:
  char* getX() const { return m_x; }
  void setX(char* x) { m_x = x; }
  T2 getY() const { return m_y; }
  void setY(T2 y) { m_y = y; }
  void display() const;

 private:
  char* m_x;
  T2 m_y;
};
template <typename T2>  // 这里需要带上模板头
void Point<char*, T2>::display() const {
  cout << "x=" << m_x << " ! y=" << m_y << endl;
}

int main() {
  int a = 10;
  int b = 20;
  cout << Max(a, b) << endl;  // 20
  STU stu1 = {"王明", 16, 95.5};
  STU stu2 = {"徐亮", 17, 90.0};
  cout << Max(stu1, stu2) << endl;  // 王明 , 16 , 95.5

  (new Point<int, int>(10, 20))->display();             // x=10, y=20
  (new Point<int, char*>(10, "东京180度"))->display();  // x=10, y=东京180度
  (new Point<char*, int>("东京180度", 10))->display();  // x=东京180度 ! y=10
  (new Point<char*, char*>("8度", "2度"))->display();   // x=8度 | y=1度

  return 0;
}
