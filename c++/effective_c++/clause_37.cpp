#include <iostream>

using namespace std;

// 绝不重新定义继承而来的缺省参数值

// 只能继承两种函数：virtual和non-virtual函数。
// 然后重新定义一个继承而来的non-virtual函数永远都是错误的。
// virtual函数是动态绑定，而缺省的参数值却是静态绑定。
// 所以可以继承的virtual函数绝不能修改缺省的参数值。

// 静态绑定又名前期绑定；动态绑定又名后期绑定。
// 对象的静态类型，就是在程序中被声明时所采用的类型。
class Shape {
 public:
  enum ShapeColor { Red, Green, Blue };
  virtual void draw(ShapeColor color = Red) const = 0;
};
class Rectangle : public Shape {
 public:
  virtual void draw(ShapeColor color = Green) const {
    cout << "Rectangle:" << color << endl;
  }
};
class Circle : public Shape {
 public:
  virtual void draw(ShapeColor color) const {
    cout << "Circle:" << color << endl;
  }
};

// 对象的所谓动态类型是指目前所指对象的类型。
// 动态类型和它的名字一样，可以动态改变（通常是通过赋值操作）。
// virtual函数是动态绑定的，
// 意思调用哪一个virtual函数取决于发出调用的哪个对象的动态类型。

// virtual函数是动态绑定，而缺省参数值确是静态绑定。意味着：
// 调用一个定义于derived class内的virtual函数时，
// 却使用base class为它所指定的缺省参数值。
// C++为什么这样运行呢？答案在于运行期效率。
// 如果缺省值是动态绑定，
// 编译器就必须有某种办法在运行期为virtual函数决定适当的参数缺省值。
// 这比在编译器决定的机制更慢且更复杂。

// 不重新定义缺省值，导致代码重复，
// 如果缺省值改变了，那么所有derived class也必须改变：
class Rectangle1 : public Shape {
 public:
  virtual void draw(ShapeColor color = Red) const {
    cout << "Rectangle:" << color << endl;
  }
};

// 使用virtual函数的代替设计，使用NVI手法：
class Shape2 {
 public:
  enum ShapeColor { Red, Green, Blue };
  void draw(ShapeColor color = Red) const { doDraw(color); }

 private:
  virtual void doDraw(ShapeColor color) const = 0;
};
class Rectangle2 : public Shape2 {
 private:
  virtual void doDraw(ShapeColor color) const {
    cout << "Rectangle2:" << color << endl;
  }
};
// non-virtual函数应该绝对不被derived class覆写，
// 所有这个设计很清楚地使得draw函数的color缺省参数总是Red。

// 请记住：
// 绝对不要重新定义一个继承而来的缺省参数值，
// 因为缺省参数值都是静态绑定，
// 而virtual函数唯一覆写的东西却是动态绑定。

int main() {
  // 静态类型为Shape*，没有动态类型，尚未指向任何类型：
  Shape *ps;

  // 静态类型为Shape*，动态类型是Circle*：
  Shape *pc = new Circle;
  pc->draw();  // Circle:0

  // 静态类型为Shape*，动态类型是Rectangle*：
  Shape *pr = new Rectangle;
  pr->draw();  // Rectangle:0

  // ps,pc和pr无论真正指向什么，它们的静态类型都是Shape*。

  // 上面的现象并不是因为是通过指针调用，通过引用调用问题仍然存在：
  Circle c;
  Rectangle r;
  Shape &s1 = c;
  Shape &s2 = r;
  s1.draw();  // Circle:0
  s2.draw();  // Rectangle:0

  return 0;
}