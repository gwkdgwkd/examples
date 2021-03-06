#include <assert.h>
#include <iostream>

using namespace std;

// 确定你的public继承模型是is-a关系

// 以C++进行面向对象编程，最重要的一个规则是：public继承意味着“is-a”的关系。把这个规则牢牢烙印在心中吧！
// 基类对象可以出现的地方，public派生类对象一样可以出现，就是所谓的liskov substitution principle。

// 在C++公有继承中，任何函数如果期望获得一个类型为Person（或指针或引用）的实参，也都愿意接受一个Student对象（或指针或引用）：
class Person {};
class Student : public Person {};
void eat(const Person& p) {}     // 任何人都会吃
void study(const Student& s) {}  // 只有学生才到校学习

// public继承和is-a之间的等价关系听起来简单，但有时候直觉可能会误导你。
class Bird {
 public:
  // 鸟可以飞
  virtual void fly();
};
class Penguin : public Bird {};  // 企鹅是一种鸟。但实际不会飞
// 应该像下面这样，这样更能反映我们真正的意思：
class Bird1 {};
class FlyingBird1 : public Bird1 {
 public:
  virtual void fly();
};
class Penguin1 : public Bird1 {};

// 另一个思想派别的想法是，为企鹅重新定义fly函数，令它产生一个运行期错误。表明“企鹅会飞，但尝试那么做是一种错误”。
// “企鹅不会飞”这一限制应该由编译器来实施。
class Bird2 {};
class Penguin2 : public Bird2 {};
// 应该采取“在编译期拒绝企鹅飞行”的设计，而不是“只在运行期才能检查错误”的设计。

// 正方形应该公有继承矩形么？应该，每个人都知道正方形是一种矩形，反之则不一定。
class Rectangle {
 public:
  virtual void setHeight(int newHeight) {}
  virtual void setWidth(int newWidth) {}
  virtual int height() const {}
  virtual int width() const {}
};
void makeBigger(Rectangle& r) {
  int oldHeight = r.height();
  r.setWidth(r.width() + 10);
  // assert永远为真，因为只改变了宽度，没有改变高度。
  assert(r.height() == oldHeight);
}
class Square : public Rectangle {};
// 某些可以作用于矩形的方法（单独修改宽度）却不能作用于正方形上。与public继承主张的思想不一致。

// 除is-a（是一个）关系外，还有has-a（有一个）和is-implemented-in-terms-of(根据某物实现出)。

// 请记住：
// public继承意味is-a。适用于base class身上的每一件事件一定也适用与derived class身上，因为每一个derived class对象也都是一个base class对象。

int main() {
  Person p;
  Student s;
  eat(p);
  eat(s);
  // study(p);  // 错误，p不是个学生
  study(s);

  Penguin2 p2;
  // p2.fly();  // 错误

  Square square;
  assert(square.width() == square.height());  // 所有正方形一定为真
  makeBigger(square);  // is-a关系，可以为其增加面积
  assert(square.width() == square.height());  // 对所有正方形应仍然为真

  return 0;
}