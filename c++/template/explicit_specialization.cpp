#include <iostream>

// C++没有办法限制类型参数的范围，可以使用任意一种类型来实例化模板。
// 但是模板中的语句（函数体或者类体）不一定就能适应所有的类型，
// 可能会有个别的类型没有意义，或者会导致语法错误。
// template<class T> const T& Max(const T& a, const T& b){ return a > b ? a : b; }
// a>b这条语句，>能够用来比较int、float、char等基本类型数据的大小，
// 但是却不能用来比较结构体变量、对象以及数组的大小，因为我们并没有针对结构体、类和数组重载>。
// 另外，该函数模板虽然可以用于指针，但比较的是地址大小，而不是指针指向的数据，所以也没有现实的意义。
// 模板是一种泛型技术，它能接受的类型是宽泛的、没有限制的，
// 并且对这些类型使用的算法都是一样的（函数体或类体一样）。
// 但是现在我们希望改变这种“游戏规则”，让模板能够针对某种具体的类型使用不同的算法（函数体或类体不同），
// 这在C++中是可以做到的，这种技术称为模板的显示具体化（Explicit Specialization）。
// 函数模板和类模板都可以显示具体化。

namespace n1 {  // 函数模板的显式具体化
template <class T>
const T& Max(const T& a, const T& b) {
  std::cout << "template:";
  return a > b ? a : b;
}

typedef struct {
  std::string name;
  int age;
  float score;
} STU;
template <>  // 函数模板针对STU类型的显示具体化
// const STU& Max<STU>(const STU& a, const STU& b) {
// Max<STU>中的STU是可选的，因为函数的形参已经表明，
// 这是STU类型的一个具体化，编译器能够逆推出T的具体类型：
const STU& Max<>(const STU& a, const STU& b) {
  std::cout << "explicit STU:";
  return a.score > b.score ? a : b;
}
std::ostream& operator<<(std::ostream& out, const STU& stu) {
  out << stu.name << "," << stu.age << "," << stu.score;
  return out;
}

// 对于给定的函数名，可以有非模板函数、模板函数、显示具体化模板函数以及它们的重载版本：
const double Max(const double& a, const double& b) {
  std::cout << "func:";
  return a > b ? a : b;
}
template <>
const double& Max<>(const double& a, const double& b) {
  std::cout << "explicit double:";
  return a > b ? a : b;
}
template <>
const float& Max<>(const float& a, const float& b) {
  std::cout << "explicit float:";
  return a > b ? a : b;
}

void func1() {
  std::cout << Max(4, 5) << std::endl;      // template:5
  std::cout << Max('a', 'c') << std::endl;  // template:c
  STU s1{"hello", 18, 80};
  STU s2 = {"word", 20, 90};
  std::cout << Max(s1, s2) << std::endl;  // explicit STU:world,20,90
}

void func2() {
  // 在调用函数时，显示具体化优先于常规模板：
  std::cout << Max(4.3f, 2.5f) << std::endl;  // explicit float:4.3
  // 而非模板函数优先于显示具体化和常规模板：
  std::cout << Max(4.3, 2.5) << std::endl;  // func:4.3
}

void test() {
  func1();
  func2();
}
}  // namespace n1

namespace n2 {  // 除了函数模板，类模板也可以显示具体化，并且它们的语法是类似的
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
template <class T1, class T2>  // 类模板的成员函数，类外定义需要带上模板头
void Point<T1, T2>::display() const {
  std::cout << "x=" << m_x << ",y=" << m_y << std::endl;
}

template <>  // 类模板针对字符串类型的显示具体化
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
// 显式具体化类模板的成员函数类外定义，不能带模板头template<>
void Point<char*, char*>::display() const {
  std::cout << "x=" << m_x << "|y=" << m_y << std::endl;
}

// 另外C++还允许只为一部分类型参数提供实参，这称为部分显式具体化。
// 部分显式具体化只能用于类模板，不能用于函数模板。
// 类名后面要列出所有的类型参数，是为了让编译器确认“到底是第几个类型参数被具体化了”：
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
template <typename T2>  // 部分显式具体化的类函数类外定义，需要带上模板头
void Point<char*, T2>::display() const {
  std::cout << "x=" << m_x << "!y=" << m_y << std::endl;
}
void test() {
  (new Point<int, int>(10, 20))->display();            // x=10,y=20
  (new Point<int, char*>(10, "xxx"))->display();       // x=10,y=xxx
  (new Point<char*, int>("yyy", 10))->display();       // x=yyy!y=10
  (new Point<char*, char*>("mmm", "nnn"))->display();  // x=mmm|y=nnn
}
}  // namespace n2

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::test();
      break;
    case 1:
      n2::test();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}