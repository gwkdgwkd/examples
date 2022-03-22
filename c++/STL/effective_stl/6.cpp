#include <fstream>
#include <iostream>
#include <iterator>
#include <list>

// 当心C++编译器最烦人的分析机制

int f1(double d);  // 声明函数
int f2(double(d));  // 和上面一样，参数d两边的括号是多余的，会被忽略
int f3(double);          // 同上，参数名被忽略
int g1(double (*pf)());  // g1以指向函数的指针为参数
int g2(double pf());     // 同上，pf为隐士指针，C和C++中都有效
int g3(double());        // 同上，省去参数名

// C++中的一条普遍规律，即尽可能地解释为函数声明。
class Widget {};
Widget w();  // 没有声明对象。声明了一个函数，不带任何参数，返回Widget

// int h((int i));  // 把形式参数的声明用括号括起来是非法的

int main() {
  std::ifstream dataFile("ints.dat");
  std::list<int> data(std::istream_iterator<int>(dataFile),
                      std::istream_iterator<int>());
  // 上面的代码可以通过编译，但是在运行时，什么也不会做。这是一个函数声明。
  // 第一个参数的名称是dataFile。它的类型是istream_iterator<int>。
  // dataFile两边的括号是多余的，会被忽略。
  // 第二个参数没有名称。
  // 它的类型是指向不带参数的函数的指针，该函数返回一个istream_iterator<int>。

  // 把形式参数的声明用括号括起来是非法的，但给函数参数加上括号却是合法的，所以通过增加一对括号：
  std::list<int> data1((std::istream_iterator<int>(dataFile)),
                       std::istream_iterator<int>());
  // 有的编译器不支持这样声明data

  // 更好的方式是在对data的声明中避免使用匿名istream_iterator对象，而是给些迭代器一个名称
  std::istream_iterator<int> dataBegin(dataFile);
  std::istream_iterator<int> dataEnd;
  std::list<int> data2(dataBegin, dataEnd);
  // 使用命名的迭代器对象与通常的STL程序风格相违背。
  // 但为了使代码对所有编译器都没有二义性，并且使维护代码的人理解起来更容易，这一代价是值得的。

  return 0;
}