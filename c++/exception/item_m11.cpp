#include <iostream>

using namespace std;

// 禁止异常信息（exceptions）传递到析构函数外

// 在有两种情况下会调用析构函数：
// 1.是在正常情况下删除一个对象，例如对象超出了作用域或被显式地delete。
// 2.异常传递的堆栈辗转开解（stack-unwinding）过程中，
//   由异常处理系统删除一个对象。
// 在上述两种情况下，调用析构函数时异常可能处于激活状态也可能没有处于激活状态。
// 遗憾的是没有办法在析构函数内部区分出这两种情况。
// 因此在写析构函数时你必须保守地假设有异常被激活。
// 因为如果在一个异常被激活的同时，析构函数也抛出异常，
// 并导致程序控制权转移到析构函数外，C++将调用terminate函数。
class Session {
 public:
  Session();
  ~Session();

 private:
  static void logCreation(Session *objAddr) {}
  static void logDestruction(Session *objAddr) {}
};
Session::~Session() { logDestruction(this); }
// 一切看上去很好，但如果logDestruction抛出一个异常，会发生什么事呢？
// 异常没有被Session的析构函数捕获住，
// 所以它被传递到析构函数的调用者那里。
// 但是如果析构函数本身的调用就是源自于某些其它异常的抛出，
// 那么terminate函数将被自动调用，彻底终止你的程序。

// 事态果真严重到了必须终止程序运行的地步了么？
// 如果没有，你必须防止在logDestruction内抛出的异常，
// 传递到Session析构函数的外面，唯一的方法是用try和catch blocks。
class Session1 {
 public:
  Session1();
  ~Session1();

 private:
  static void logCreation(Session1 *objAddr) {}
  static void logDestruction(Session1 *objAddr) {}
};
Session1::~Session1() {
  try {
    logDestruction(this);
  } catch (...) {
    cerr << "Unable to log destruction of Session object at address " << this
         << ".\n";
  }
}
// 但是这样做并不比你原来的代码安全。
// 如果在catch中调用operator<<时导致一个异常被抛出，
// 就又遇到了老问题，一个异常被转递到Session析构函数的外面。
// 可以在catch中放入try，但是这总得有一个限度，否则会陷入循环。
// 因此在释放Session时必须忽略掉所有它抛出的异常：
class Session2 {
 public:
  Session2();
  ~Session2();

 private:
  static void logCreation(Session2 *objAddr) {}
  static void logDestruction(Session2 *objAddr) {}
};
Session2::~Session2() {
  try {
    logDestruction(this);
  } catch (...) {
  }
}
// catch表面上好像没有做任何事情，这是一个假象，
// 实际上它阻止了任何从logDestruction抛出的异常，
// 被传递到session析构函数的外面。
// 无论session对象是不是在堆栈退栈（stack unwinding）中被释放，
// terminate函数都不会被调用。

// 不允许异常传递到析构函数外面还有第二个原因。
// 如果一个异常被析构函数抛出而没有在函数内部捕获住，
// 那么析构函数就不会完全运行（它会停在抛出异常的那个地方上）。
// 如果析构函数不完全运行，它就无法完成希望它做的所有事情。

// 禁止异常传递到析构函数外有两个原因：
// 1.能够在异常转递的堆栈辗转开解（stack-unwinding）的过程中，
//   防止terminate被调用。
// 2.它能帮助确保析构函数总能完成我们希望它做的所有事情。

int main() { return 0; }