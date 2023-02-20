#include <iostream>
#include <string>

// 尽量少做转型动作

// C++规则的设计目标之一是，保证类型错误绝不可能发生。
// 转型破坏来类型系统，可能导致任何种类的麻烦，有些容易识别，有些非常隐晦。
// 来自C，Java或C#的开发人员，需要特别注意。
// 那些语言中转型比较必要而无法避免，也比较不危险（于C++相比）。

// 转型语法的三种形式：
// 1.C风格的转型：(T)expression
// 2.函数风格的转型：T(expression)
// 上面两种形式并无差别，纯粹只是小括号的摆放位置不同而已，统称为旧式转型。
// 3.C++还提供四种新式转型，常常被称为new-style或c++ style cast。

// C++的转型：
// 1.const_cast<T>(expression)
//   通常被用来将对象的常量性删除，唯一有此能力的转型操作符。
// 2.dynamic_cast<T>(expression)
//   主要用来执行安全向下转型，用来决定某对象是否归属继承体系中的某个类型，
//   唯一无法由旧式语法执行的动作，也是唯一可能耗费重大运行成本的转型动作。
// 3.reinterpret_cast<T>(expression)
//   执行低级转型，实际动作及结果可能取决于编译器，表示它不可以移植。
// 4.static_cast<T>(expression)
//   强迫隐式转换，将non-const转换为const，或将int转为double等等。
//   无法将const转为non-const，这个只有const_cast才能做到。

// 旧式转型仍然合法，但新式转型比较受欢迎，因为：
// 1.容易在代码中被识别出来，便于定位问题;
// 2.各转型动作的目标愈窄化，编译器可能诊断出错误的运用。

// 转型其实什么都没做，只是告诉编译器把某种类型视为另一种类型，这是错误的观念。
// 不论是通过转型操作而进行的显示转换，或通过编译器完成的隐式转换，
// 任何一个类型转换往往真的令编译器编译出运行期间执行的码。
// 单一对象可能拥有一个以上的地址，C，Java，C#都不可能发生这种事，
// 但C++可能，实际上一旦使用多继承，这事几乎一直发生着，即使在单继承中也可能发生。
// 例如以Base*指向它时的地址和以Derived*指向它时的地址。
class Base {};
class Derived : public Base {};

// 另一个关于转型的事：
class Window {
 public:
  virtual void onResize() {}
};
class SpecialWindow : public Window {
 public:
  virtual void onResize() {
    // 调用的不是当前对象上的函数，
    // 而是稍早转型动作建立的那个*this对象base部分临时对象的onResize。
    // 虽然成员函数只有一份，但是成员函数都有个隐藏的this指针，
    // 会影响成员操作的数据。
    static_cast<Window>(*this).onResize();
    // SpecialWindow专属行为
  }
};
class SpecialWindow1 : public Window {
 public:
  virtual void onResize() {
    // 调用base class版本的onResize函数：
    Window::onResize();
    // SpecialWindow专属行为
  }
};

// dynameic_cast的许多实现版本执行速度相当慢，
// 需要调用多次strcmp，根据继承的层次决定的。
// 之所以需要dynameic_cast，
// 通常是因为想在derived class对象上执行derived class操作函数，
// 但手上却只有一个指向base的指针或引用。
// 有两个一般性做法可以避免这个问题：
// 1.使用容器并在其中存储执行derived class对象的指针;
// 2.在base class内提供virtual函数，
//   可以通过base class指针或引用处理所有可能的各种派生类。

// 绝对必须避免的是所谓的连串dynamic_cast，
// 有好多分支，每个分支都要执行dynameic_cast，可以用虚函数代替。
// 优良的C++代码很少使用转型，但若要说完全摆脱它们又太过不切实际。

// 请记住：
// 1.如果可以，尽量避免转型，特别是在注重效率的代码中避免dynamic_cast；
// 2.如果有个设计需要转型动作，试着发展无需转型的代替设计；
// 3.如果转型是必要的，试着将它隐藏于某个函数背后；
// 4.客户随后可以调用该函数，而不需要将转型放进自己的代码内；
// 5.宁可使用C++风格转型，不要使用旧式转型，
//   前者很容易辨别出来，而且职责已经分好类了。

int main() {
  Derived d;
  Base *pb = &d;

  return 0;
}