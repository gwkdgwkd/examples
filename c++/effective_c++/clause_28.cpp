#include <iostream>
#include <memory>

using namespace std;

// 避免返回handles指向对象内部成分

class Point {
 public:
  Point(int x, int y) : a(x), b(y) {}
  void setX(int newVal) { a = newVal; }
  void setY(int newVal) { b = newVal; }
  void display() { cout << "x:" << a << ",y:" << b << endl; }

 private:
  int a, b;
};
struct RectData {
  Point ulhc;
  Point lrhc;
  RectData(Point x, Point y) : ulhc(x), lrhc(y) {}
};
class Rectangle {
 public:
  Rectangle(Point ul, Point lr) : pData(new RectData(ul, lr)) {}
  void display() const {
    pData->ulhc.display();
    pData->lrhc.display();
  }

  // Rectangle的客户必须能够计算Rectangle的范围，所以提供下面两个函数：
  Point &upperLeft() const { return pData->ulhc; }
  Point &lowerRight() const { return pData->lrhc; }
  // 根据条款20（以引用的方式传递用户自定义类型往往比by value方式传递更高效），
  // 所以上面函数返回引用。
  // 这样设计可以通过编译，但却是错误的，实际上它是自我矛盾的：
  // 1.一方面upperLefr和lowerRight被声明为const成员函数，
  //   因为只是为了提供获取的坐标点的方法，而不让客户修改。
  // 2.另一方面两个函数都返回引用指向内部数据，
  //   调用者可以通过这些引用更改内部数据。

  // 两个教训：
  // 1.成员变量的封装性最多只等于返回其reference的函数的访问级别。
  //   ulhc和lrhc被private的pData隐藏，但实际是public的，
  //   因为upperLefr和lowerRight传出了它们的引用。
  // 2.如果const成员函数传出一个引用，后者所指数据与对象自身有关联，
  //   而它又被存储于对象之外，那么调用者可以修改那笔数据。
  //   这是bitwise constness的（条款3）的一个附带结果。

  // 成员函数返回指针或迭代器，相同的情况还是会发生，原因也相同。
  // 引用、指针和迭代器都是所谓的handles（号码牌，用来取得某个对象），
  // 而返回一个代表对象内部数据的handle，
  // 随之而来的便是降低对象封装性的风险。

  // 除了成员变量，不是public的成员函数也是对象内部的一部分。
  // 这意味着绝对不应该令成员函数返回一个指针指向访问级别低的成员函数，
  // 否则后者的实际访问级别就会提高如前者。
  typedef void (*testPtr)();
  static testPtr Test() { return Rectangle::test; }

 private:
  shared_ptr<RectData> pData;
  static void test() { cout << "test" << endl; }
};

class Rectangle1 {
 public:
  // 返回const引用就不能修改了：
  const Point &upperLeft() const { return pData->ulhc; }
  const Point &lowerRight() const { return pData->lrhc; }
  // 即使如此，upperLefr和lowerRight还是返回了代表对象内部的handles，
  // 有可能在其他场合带来问题。
  // 更明确地说，它可能导致dangling handles（悬空的号码牌）：
  // 这种handle所指东西的所属对象不复存在。

 private:
  shared_ptr<RectData> pData;
};

// 这种不复存在的对象最常见的来源就是函数返回值。
class GUIObject {};
const Rectangle1 boundingBox(const GUIObject &obj){};

// 这就是为什么函数如果返回一个handle代表对象内部成分总是危险的原因。
// 1.不论这个所谓的handle是个指针或迭代器或引用；
// 2.也不论这个handle是否为const；
// 3.也不论那个返回handle的成员函数是否为const。
// 唯一关键是，有handle被传出去了，
// 一旦如此就暴露在handle比其所指对象更长寿的风险下。
// 这并不意味绝对不让成员函数返回handle，有时候必须那么做。
// 尽管如此，这样的函数毕竟是例外，不是常态。
// 例如operator[]就允许你采摘string和vector的个别元素，
// 而这些operator[]就是返回引用指向容器内的数据，
// 那些数据随容器被销毁而销毁。

// 请记住：
// 1.避免返回handles（包括引用、指针、迭代器）指向对象内部。
// 2.遵守这个条款可增加封装性，帮助const成员函数的行为像个const，
//   并将发生虚吊号码牌的可能性降至最低。

int main() {
  Point coord1(0, 0);
  Point coord2(100, 100);
  const Rectangle rec(coord1, coord2);
  rec.display();
  // x:0,y:0
  // x:100,y:100
  rec.upperLeft().setX(50);
  rec.display();
  // x:50,y:0
  // x:100,y:100

  // rec.test();  // 私有函数不能访问
  Rectangle::testPtr ptr = rec.Test();  // 能获取私有函数的指针
  ptr();                                // test

  const Rectangle1 rec1;
  // rec1.upperLefr().setX(50);  // 不能修改了

  // boundingBox的调用获得一个新的、暂时的Rectangle1对象。
  // 没有名称，用temp表示。
  // 随后temp调用upperLeft，
  // 返回一个引用指向temp的一个内部成分，也就是Point。
  // 于是pUpperLeft指向那个Point对象。
  // 语句结束后，boundingBox的返回值，也就是那个temp，
  // 将被销毁，间接导致temp内的Point析构。
  // 最终导致pUpperLeft指向一个不再存在的对象。
  // 也就是说产出pUpperLeft哪个语句结束，
  // pUpperLeft就变成悬空、虚吊（dangling）。
  GUIObject *pgo;
  const Point *pUpperLeft = &(boundingBox(*pgo).upperLeft());

  return 0;
}