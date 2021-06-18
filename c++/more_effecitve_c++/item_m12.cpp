#include <iostream>

using namespace std;

// 理解“抛出一个异常”与“传递一个参数”或“调用一个虚函数”间的差异

// 从语法上看，在函数里声明参数与在catch子句中声明参数几乎没有什么差别：
class Widget {};  // 一个类，具体是什么类在这里并不重要
// 一些函数，其参数分别为Widget,Widget&,或Widget*
void f1(Widget w);
void f2(Widget& w);
void f3(const Widget& w);
void f4(Widget* pw);
void f5(const Widget* pw);
// 一些catch子句，用来捕获异常，异常的类型为Widget,Widget&,或Widget*
// catch (Widget w)
// catch (Widget& w)
// catch (const Widget& w)
// catch (Widget *pw)
// catch (const Widget *pw)
// 因此可能会认为用throw抛出一个异常到catch子句中与通过函数调用传递一个参数两者基本相同。这里面确有一些相同点，但是他们也存在着巨大的差异。
// 传递函数参数与异常的途径可以是传值、传递引用或传递指针，这是相同的。
// 但是当你传递参数和异常时，系统所要完成的操作过程则是完全不同的。产生这个差异的原因是：
// 你调用函数时，程序的控制权最终还会返回到函数的调用处，但是当你抛出一个异常时，控制权永远不会回到抛出异常的地方。
istream operator>>(istream& s, Widget& w) {}
void passAndThrowWidget() {
  Widget localWidget;
  cin >> localWidget;  // 传递localWidget到operator>>
  throw localWidget;   // 抛出localWidget异常
}
// 当传递localWidget到函数operator>>里，不用进行拷贝操作，而是把operator>>内的引用类型变量w指向localWidget，任何对w的操作实际上都施加到localWidget上。
// 这与抛出localWidget异常有很大不同。
// 不论通过传值捕获异常还是通过引用捕获（不能通过指针捕获这个异常，因为类型不匹配）都将进行lcalWidget的拷贝操作，也就说传递到catch子句中的是localWidget的拷贝。
// 必须这么做，因为当localWidget离开了生存空间后，其析构函数将被调用。如果把localWidget本身（而不是它的拷贝）传递给catch子句，
// 这个子句接收到的只是一个被析构了的Widget，一个Widget的“尸体”。这是无法使用的。
// 因此C++规范要求被做为异常抛出的对象必须被复制。
// 即使被抛出的对象不会被释放，也会进行拷贝操作。
void passAndThrowWidget1() {
  static Widget localWidget;  // 现在是静态变量（static）,一直存在至程序结束
  cin >> localWidget;  // 象以前那样运行
  throw localWidget;   // 仍将对localWidget进行拷贝操作
}

// 当抛出异常时仍将复制出localWidget的一个拷贝。这表示即使通过引用来捕获异常，也不能在catch块中修改localWidget；仅仅能修改localWidget的拷贝。
// 对异常对象进行强制复制拷贝，这个限制有助于我们理解参数传递与抛出异常的第二个差异：抛出异常运行速度比参数传递要慢。

// 当异常对象被拷贝时，拷贝操作是由对象的拷贝构造函数完成的。
// 该拷贝构造函数是对象的静态类型（static type）所对应类的拷贝构造函数，而不是对象的动态类型（dynamic type）对应类的拷贝构造函数。
class SpecialWidget : public Widget {};
void passAndThrowWidget2() {
  SpecialWidget localSpecialWidget;
  // rw引用SpecialWidget它抛出一个类型为Widget的异常
  Widget& rw = localSpecialWidget;
  // 这里抛出的异常对象是Widget，即使rw引用的是一个SpecialWidget。因为rw的静态类型（static type）是Widget，而不是SpecialWidget。
  // 这种行为可能与你所期待的不一样，但是这与在其他情况下C++中拷贝构造函数的行为是一致的。
  throw rw;
}

int main() { return 0; }