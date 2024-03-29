#include <iostream>
#include <memory>

// 以独立语句将newed对象置入智能指针

class Widget {
 public:
  Widget() { std::cout << "Widget" << std::endl; }
  ~Widget() { std::cout << "~Widget" << std::endl; }
};
int priority() {}
void processWidget(std::shared_ptr<Widget> pw, int priority) {}

// 请记住：
// 1.以独立语句将newed对象存储于（置入）智能指针内；
// 2.如果不这样做，一旦异常被抛出，有可能导致难以察觉的资源泄露。

int main() {
  // processWidget(new Widget, priority());  // 编译失败
  // shared_ptr构造函数需要一个原始指针，
  // 但该构造函数是个explicit构造函数，无法进行隐式转换。

  // 需要将new Widget的原始指针转换为processWidget所要求的shared_ptr：
  processWidget(std::shared_ptr<Widget>(new Widget), priority());
  // 上面语句可以通过编译，虽然使用对象管理式资源，但却可能泄露资源。

  // 编译器生成processWidget调用码之前，必须首先计算即将被传递的各个实参。
  // 第二个只是一个单纯的函数调用：调用priority；
  // 但第一个实参shared_ptr<Widget>(new Widget)由两部分组成：
  // 1.执行new Widget表达式；
  // 2.调用shared_ptr构造函数。
  // C++编译器以什么样的次序完成这些事呢？
  // 弹性很大，java和C#总是以特定次序完成。
  // 可以确定的是，new Widget一定执行于shared_ptr构造函数被调用之前；
  // 但对priority的调用可以排在1，2，3都行。
  // 如果第2步调用并产生了异常，会发生什么事？
  // new Widget返回的指针将会遗失，因为尚为放到shared_ptr内。

  // 避免这类问题的办法很简单，使用分离语句，在单独语句内以智能指针存储newed对象：
  std::shared_ptr<Widget> pw(new Widget);
  processWidget(pw, priority());  // 不会造成泄露
  // 因为编译器对跨越语句的各项操作没有重新排列的自由，只有在语句内它才能排列。

  return 0;
}