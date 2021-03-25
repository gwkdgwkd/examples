#include <iostream>

using namespace std;

// 模板（Templet）并不是真正的函数或类，它仅仅是编译器用来生成函数或类的一张“图纸”。模板不会占用内存，最终生成的函数或者类才会占用内存。
// 由模板生成函数或类的过程叫做模板的实例化（Instantiate），相应地，针对某个类型生成的特定版本的函数或类叫做模板的一个实例（Instantiation）。
// 模板的实例化是按需进行的，用到哪个类型就生成针对哪个类型的函数或类，不会提前生成过多的代码。也就是说，编译器会根据传递给类型参数的实参（也可以是编译器自己
// 推演出来的实参）来生成一个特定版本的函数或类，并且相同的类型只生成一次。实例化的过程也很简单，就是将所有的类型参数用实参代替。
template <typename T>
void Swap(T &a, T &b) {
  T temp = a;
  a = b;
  b = temp;
}
// 编译器会根据不同的实参实例化出不同版本的Swap()函数。对于Swap(n1, n2)调用，编译器会生成并编译一个Swap()版本，其中T被替换为int：
// void Swap(int &a, int &b) {
//   int temp = a;
//   a = b;
//   b = temp;
// }

// 另外需要注意的是类模板的实例化，通过类模板创建对象时并不会实例化所有的成员函数，只有等到真正调用它们时才会被实例化；如果一个成员函数永远不会被调用，那它就永
// 远不会被实例化。这说明类的实例化是延迟的、局部的，编译器并不着急生成所有的代码。
// 通过类模板创建对象时，一般只需要实例化成员变量和构造函数。成员变量被实例化后就能够知道对象的大小了（占用的字节数），构造函数被实例化后就能够知道如何初始化了；
// 对象的创建过程就是分配一块大小已知的内存，并对这块内存进行初始化。
template <class T1, class T2>
class Point {
 public:
  Point(T1 x, T2 y) : m_x(x), m_y(y) {}

 public:
  T1 getX() const { return m_x; }
  void setX(T1 x) { m_x = x; }
  T2 getY() const { return m_y; };
  void setY(T2 y) { m_y = y; };
  void display() const;

 private:
  T1 m_x;
  T2 m_y;
};
template <class T1, class T2>
void Point<T1, T2>::display() const {
  cout << "x=" << m_x << ", y=" << m_y << endl;
}

int main() {
  int n1 = 100, n2 = 200, n3 = 300, n4 = 400;
  float f1 = 12.5, f2 = 56.93;
  Swap(n1, n2);  // T为int，实例化出void Swap(int &a, int &b);
  Swap(f1, f2);  // T为float，实例化出void Swap(float &a, float &b);
  Swap(n3, n4);  // T为int，调用刚才生成的void Swap(int &a, int &b);

  // p1调用了所有的成员函数，整个类会被完整地实例化。p2只调用了构造函数和display()函数，剩下的get函数和set函数不会被实例化。
  Point<int, int> p1(10, 20);
  p1.setX(40);
  p1.setY(50);
  cout << "x=" << p1.getX() << ", y=" << p1.getY() << endl;  // x=40, y=50
  Point<char *, char *> p2("180度", "210度");  // x=180度, y=210度
  p2.display();
  // 值得提醒的是，Point<int, int>和Point<char*, char*>是两个相互独立的类，它们的类型是不同的，不能相互兼容，也不能自动地转换类型，所以诸如
  // p1 = p2;  // 语句是错误的，除非重载了=运算符。

  return 0;
}