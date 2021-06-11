#include <iostream>

using namespace std;

// 了解隐式接口和编译期多态

// 面向对象编程世界总是以显式接口和运行期多态解决问题。
class Widget {
 public:
  Widget() {}
  virtual ~Widget() {}
  virtual size_t size() const {}
  virtual void normalize() {}
  void swap(Widget& other) {}
};
void doProcessing1(Widget& w) {
  if (w.size() > 10) {  // 还有w!=someNastyWidget
    Widget temp(w);
    temp.normalize();
    temp.swap(w);
  }
  // w的类型被声明为Widget，所以w必须支持Widget接口。可以在源码中找到这个接口，这是一个显式接口，也就是它在源码中明确可见。
  // Widget的某些成员函数是virtual，w对那些函数的调用将表现出运行期多态。运行期根据w的动态类型决定调用哪一个函数。

  // T(w的类型)的隐式接口看起来像是这些约束：
  //  它必须提供一个名为size的成员函数，并返回一个整数。
  //  它必须支持一个operator!=函数，用来比较两个T对象。

  // 操作符重载带来的可能性：这两个约束都不需要满足。
  // T必须支持size成员函数，然而这个函数也可能从base class继承。这个成员函数不需返回一个整数值，甚至不需要返回一个数值类型。
  // 甚至不需要返回一个定义有operator的类型。唯一需要做的是，返回一个类型为x的对象，而x对象加上一个int（10的类型）必须能够
  // 调用一个operator>。这个operator>不需要非得取得一个类型为x的参数不可，因为它也可以取得类型Y，只要存在隐转换就可以。
  // 同样的道理，T并不需要支持operator!=。
  // operator&&被重载，表达式的意义被改变了。

  // template参数身上的隐式接口，就像class对象身上的显示接口一样真实，而且两者都在编译期完成检查。
  // 就像你无法以一种“与class提供的显示接口矛盾”的方式来使用对象（编译不过），
  // 也无法在template中使用“不支持template所要求的隐式接口”的对象(一样编译不过)。
}

// template及泛型编程的世界，与面向对象有根本的不同。
// 在此世界中显式接口和运行期多态仍然存在，但重要性降低。反倒是隐式接口和编译器多态移到了前头。
template <typename T>
void doProcessing2(T& w) {
  if (w.size() > 10) {
    T temp(w);
    temp.normalize();
    temp.swap(w);
  }
  // w必须支持哪种接口，由执行w身上的操作来决定。w在函数中使用的方法就是T必须支持的一组隐式接口。
  // 凡涉及w的任何函数调用，例如operator>和operator!=，有可能造成template具现化（发生在编译期），使得调用得以成功。
  // 以不同的template参数具现化会导致调用不同的函数，这便是所谓的编译期多态。
}

// 运行期多态和编译期多态之间的差异，类似于“哪一个重载函数被调用（编译期）”和“哪一个virtual函数该被绑定（运行期）”之间的差异。
// 通常显式接口由函数的签名（函数名称、参数类型、返回类型）构造。
// 隐式接口就完全不同了。并不基于函数签名，而是由表达式组成。

// 请记住：
// class和template都支持接口和多态。
// 对class而言接口是显式的，以函数签名为中心。多态则是通过virtual函数发生于运行期。
// 对template而言，接口是隐式的，基于有效表达式。多态则是通过template具现化和函数重载解析发生于编译器。

int main() { return 0; }