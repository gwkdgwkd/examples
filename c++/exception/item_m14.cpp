#include <exception>
#include <iostream>

using namespace std;

// 审慎使用异常规格(exception specifications)

// 毫无疑问，异常规格是一个引人注目的特性。
// 它使得代码更容易理解，因为它明确地描述了一个函数可以抛出什么样的异常。
// 但是它不只是一个有趣的注释，编译器在编译时有时能够检测到异常规格的不一致。
// 而且如果一个函数抛出一个不在异常规格范围里的异常，
// 系统在运行时能够检测出这个错误，然后一个特殊函数unexpected将被自动地调用。
// 异常规格既可以做为一个指导性文档同时也是异常使用的强制约束机制，
// 它好像有着很诱人的外表。
// 函数unexpected缺省的行为是调用函数terminate，
// 而terminate缺省的行为是调用函数abort，
// 所以一个违反异常规格的程序其缺省的行为就是halt（停止运行）。
// 在激活的栈中的局部变量没有被释放，
// 因为abort在关闭程序时不进行这样的清除操作。
// 对异常规格的触犯变成了一场并不应该发生的灾难。
// 不幸的是，我们很容易就能够编写出导致发生这种灾难的函数。
// 编译器仅仅部分地检测异常的使用是否与异常规格保持一致。
// 一个函数调用了另一个函数，并且后者可能抛出一个违反前者异常规格的异常，
// 编译器不对此种情况进行检测，并且语言标准也禁止编译器拒绝这种调用方式：
void f1() {}
void f2() throw(int) {
  f1();  // 即使f1可能抛出不是int类型的异常，这也是合法的
}

// 一种好方法是避免在带有类型参数的模板内使用异常规格：
template <class T>
bool operator==(const T& lhs, const T& rhs) throw() {
  return &lhs == &rhs;
}
// 这个模板包含的异常规格表示模板生成的函数不能抛出异常。
// opertor&可能会抛出一个异常，这样就违反了我们的异常规格，
// 使得程序控制跳转到unexpected。
// 几乎不可能为一个模板提供一个有意义的异常规格，
// 因为模板总是采用不同的方法使用类型参数。
// 解决方法只能是模板和异常规格不要混合使用。

// 能够避免调用unexpected函数的第二个方法是：
// 如果在一个函数内调用其它没有异常规格的函数时应该去除这个函数的异常规格。
typedef void (*CallBackPtr)(int eventXLocation, int eventYLocation,
                            void* dataToPassBack);
class CallBack {
 public:
  CallBack(CallBackPtr fPtr, void* dataToPassBack)
      : func(fPtr), data(dataToPassBack) {}
  void makeCallBack(int eventXLocation, int eventYLocation) const throw();

 private:
  CallBackPtr func;  // function to call when callback is made
  void* data;        // data to pass to callback function
};
void CallBack::makeCallBack(int eventXLocation, int eventYLocation) const
    throw() {
  // 在makeCallBack内调用func，要冒违反异常规格的风险，
  // 因为无法知道func会抛出什么类型的异常。
  func(eventXLocation, eventYLocation, data);
}
// 通过在程序在CallBackPtr typedef中采用更严格的异常规格来解决问题：
// typedef void (*CallBackPtr1)(int eventXLocation,
//                              int eventYLocation,
//                              void* dataToPassBack) throw();
// 这样定义typedef后，如果注册一个可能会抛出异常的callback函数将是非法的：
// void callBackFcn1(int eventXLocation,
//                   int eventYLocation, void* dataToPassBack);
// void* callBackData() {}
// CallBack c1(callBackFcn1, callBackData);
// 带有异常规格的回调函数：
// void callBackFcn2(int eventXLocation,
//                   int eventYLocation, void* dataToPassBack) throw();
// 正确，callBackFcn2没有异常规格：
// CallBack c2(callBackFcn2, callBackData);
// 传递函数指针时进行这种异常规格的检查，
// 是语言的较新的特性，所以有可能你的编译器不支持这个特性。
// 如果它们不支持，那就依靠你自己来确保不能犯这种错误。

// 避免调用unexpected的第三个方法是处理系统本身抛出的异常。
// 这些异常中最常见的是bad_alloc，
// 当内存分配失败时它被operator new和operator new[]抛出。
// 如果你在函数里使用new操作符，
// 你必须为函数可能遇到bad_alloc异常作好准备。

// 虽然防止抛出unexpected异常是不现实的，
// 但是C++允许你用其它不同的异常类型替换unexpected异常，
// 你能够利用这个特性。
// 所有的unexpected异常对象被替换为这种类型对象：
class UnexpectedException {};
// 如果一个unexpected异常被抛出，这个函数被调用：
void convertUnexpected() { throw UnexpectedException(); }
// 当你这么做了以后，一个unexpected异常将触发调用convertUnexpected函数。
// Unexpected异常被一种UnexpectedException新异常类型替换。
// 如果被违反的异常规格包含UnexpectedException异常，
// 那么异常传递将继续下去，好像异常规格总是得到满足。
// 如果异常规格没有包含UnexpectedException，
// terminate将被调用，就好像你没有替换unexpected一样。
// 另一种把unexpected异常转变成知名类型的方法是替换unexpected函数，
// 让其重新抛出当前异常，这样异常将被替换为bad_exception。
// 你可以这样编写：
void convertUnexpected1() {
  // 如果一个unexpected异常被抛出，这个函数被调用
  throw;  // 它只是重新抛出当前
}
// 如果这么做，
// 应该在所有异常规格里包含bad_exception或它的基类，标准类exception。
// 你将不必再担心如果遇到unexpected异常会导致程序运行终止。
// 任何不听话的异常都将被替换为bad_exception，
// 这个异常代替原来的异常继续传递。

// 以全面的角度去看待异常规格是非常重要的。
// 它们提供了优秀的文档来说明一个函数抛出异常的种类，并且在违反它的情况下，
// 会有可怕的结果，程序被立即终止，在缺省时它们会这么做。
// 同时编译器只会部分地检测它们的一致性，所以他们很容易被不经意地违反。
// 而且他们会阻止high-level异常处理器来处理unexpected异常，
// 即使这些异常处理器知道如何去做。
// 综上所述，异常规格是一个应被审慎使用的特性。
// 在把它们加入到你的函数之前，应考虑它们所带来的行为是否就是你所希望的行为。

int main() {
  // 通过用convertUnexpected函数替换缺省的unexpected函数，
  // 来使上述代码开始运行：
  set_unexpected(convertUnexpected);

  return 0;
}