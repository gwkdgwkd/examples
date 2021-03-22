#include <iostream>

using namespace std;

// 在C++中，可以将虚函数声明为纯虚函数，语法格式为：
// virtual 返回值类型 函数名 (函数参数) = 0;
// 纯虚函数没有函数体，只有函数声明，在虚函数声明的结尾加上=0，表明此函数为纯虚函数。最后的=0并不表示函数返回值为0，它只起形式上的作用，告诉编译系统“这是纯虚函数”。
// 包含纯虚函数的类称为抽象类（Abstract Class）。之所以说它抽象，是因为它无法实例化，也就是无法创建对象。
// 原因很明显，纯虚函数没有函数体，不是完整的函数，无法调用，也无法为其分配内存空间。
// 抽象类通常是作为基类，让派生类去实现纯虚函数。派生类必须实现纯虚函数才能被实例化。
// 在实际开发中，你可以定义一个抽象基类，只完成部分功能，未完成的功能交给派生类去实现（谁派生谁实现）。这部分未完成的功能，往往是基类不需要的，或者在基类中无法实现的。
// 虽然抽象基类没有完成，但是却强制要求派生类完成，这就是抽象基类的“霸王条款”。
// 抽象基类除了约束派生类的功能，还可以实现多态。这或许才是C++提供纯虚函数的主要目的。
// 关于纯虚函数的几点说明:
//  1) 一个纯虚函数就可以使类成为抽象基类，但是抽象基类中除了包含纯虚函数外，还可以包含其它的成员函数（虚函数或普通函数）和成员变量。
//  2) 只有类中的虚函数才能被声明为纯虚函数，普通成员函数和顶层函数均不能声明为纯虚函数。

class Line {
 public:
  Line(float len);
  virtual float area() = 0;
  virtual float volume() = 0;

 protected:
  float m_len;
};
Line::Line(float len) : m_len(len) {}
class Rec : public Line {
 public:
  Rec(float len, float width);
  float area();

 protected:
  float m_width;
};
Rec::Rec(float len, float width) : Line(len), m_width(width) {}
float Rec::area() { return m_len * m_width; }
class Cuboid : public Rec {
 public:
  Cuboid(float len, float width, float height);
  float area();
  float volume();

 protected:
  float m_height;
};
Cuboid::Cuboid(float len, float width, float height)
    : Rec(len, width), m_height(height) {}
float Cuboid::area() {
  return 2 * (m_len * m_width + m_len * m_height + m_width * m_height);
}
float Cuboid::volume() { return m_len * m_width * m_height; }
class Cube : public Cuboid {
 public:
  Cube(float len);
  float area();
  float volume();
};
Cube::Cube(float len) : Cuboid(len, len, len) {}
float Cube::area() { return 6 * m_len * m_len; }
float Cube::volume() { return m_len * m_len * m_len; }

int main() {
  Line *p = new Cuboid(10, 20, 30);
  cout << "The area of Cuboid is " << p->area() << endl;
  cout << "The volume of Cuboid is " << p->volume() << endl;
  // The area of Cuboid is 2200
  // The volume of Cuboid is 6000

  p = new Cube(15);
  cout << "The area of Cube is " << p->area() << endl;
  cout << "The volume of Cube is " << p->volume() << endl;

  // The area of Cube is 1350
  // The volume of Cube is 3375

  return 0;
}