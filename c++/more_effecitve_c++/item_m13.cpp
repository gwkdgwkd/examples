#include <exception>
#include <iostream>

using namespace std;

// 通过引用（reference）捕获异常

// 当你写一个catch子句时，必须确定让异常通过何种方式传递到catch子句里。你可以有三个选择：
// 与你给函数传递参数一样，通过指针（by pointer），通过传值（by value）或通过引用（by reference）

// 从throw处传递一个异常到catch子句是一个缓慢的过程，在理论上这种方法的实现对于这个过程来说是效率最高的。
// 因为在传递异常信息时，只有采用通过指针抛出异常的方法才能够做到不拷贝对象。
class MyException : public exception {
 public:
  MyException() { cout << "MyException" << endl; }
};
void someFunction() {
  static MyException ex;  // 异常对象
  cout << "throw MyException" << endl;
  throw & ex;  // 抛出一个指针，指向ex
}
void doSomething() {
  try {
    someFunction();            // 抛出一个exception*
  } catch (MyException *ex) {  // 捕获exception*，没有对象被拷贝
    cout << "catch MyException" << endl;
  }
}
// 看上去很不错，但是实际情况却不是这样。为了能让程序正常运行，程序员定义异常对象时必须确保当程序控制权离开抛出指针的函数后，对象还能够继续生存。
// 全局与静态对象都能够做到这一点，但是程序员很容易忘记这个约束：
void someFunction1() {
  MyException ex;  // 局部异常对象，当退出函数的生存空间时这个对象将被释放。
  // 这简直糟糕透了，因为处理这个异常的catch子句接受到的指针，其指向的对象已经不再存在。
  throw & ex;  // 抛出一个指针，指向已被释放的对象
}
// 另一种抛出指针的方法是建立一个堆对象（new heap object）：
void someFunction2() {
  throw new MyException;  // 抛出一个指针，指向一个在堆中建立的对象(希望操作符new自己不要再抛出一个异常!)
}
// 这避免了捕获一个指向已被释放对象的指针的问题，但是catch子句的作者又面临一个令人头疼的问题：
// 他们是否应该删除他们接受的指针？如果是在堆中建立的异常对象，那他们必须删除它，否则会造成资源泄漏。
// 如果不是在堆中建立的异常对象，他们绝对不能删除它，否则程序的行为将不可预测。该如何做呢？
// 这是不可能知道的。一些被调用者可能会传递全局或静态对象的地址，另一些可能传递堆中建立的异常对象的地址。
// 通过指针捕获异常，将遇到一个哈姆雷特式的难题：是删除还是不删除？这是一个难以回答的问题。所以你最好避开它。
// 而且，通过指针捕获异常也不符合 C++语言本身的规范。
// 四个标准的异常，都不是指向对象的指针，所以你必须通过值或引用来捕获它们。
//  bad_alloc，当operator new不能分配足够的内存时，被抛出；
//  bad_cast，当dynamic_cast针对一个引用reference操作失败时，被抛出；
//  bad_typeid，当dynamic_cast 对空指针进行操作时，被抛出；
//  bad_exception，用于unexpected异常；

// 通过值捕获异常（catch-by-value）可以解决上述的问题，例如异常对象删除的问题和使用标准异常类型的问题。但是当它们被抛出时系统将对异常对象拷贝两次。
// 而且它会产生slicing problem，即派生类的异常对象被做为基类异常对象捕获时，那它的派生类行为就被切掉了（sliced off）。
// 这样的sliced对象实际上是一个基类对象：它们没有派生类的数据成员，而且当本准备调用它们的虚拟函数时，系统解析后调用的却是基类对象的函数。
// 当一个对象通过传值方式传递给函数，也会发生一样的情况。
class BaseException : public exception {
 public:
  virtual void print() { cout << "BaseException::print" << endl; }
};
class DerivedException : public BaseException {
 public:
  DerivedException() { cout << "DerivedException" << endl; }
  virtual void print() { cout << "DerivedException::print" << endl; }
};
void someFunction3() { throw DerivedException(); }
void doSomething3() {
  try {
    someFunction3();
  } catch (BaseException ex) {  // 捕获所有标准异常类或它的派生类
    ex.print();                 // 调用BaseException::print()
  }
}

// 最后剩下方法就是通过引用捕获异常（catch-by-reference）。通过引用捕获异常能使你避开上述所有问题。
// 不象通过指针捕获异常，这种方法不会有对象删除的问题而且也能捕获标准异常类型。
// 也不象通过值捕获异常，这种方法没有slicing problem，而且异常对象只被拷贝一次。
void doSomething4() {
  try {
    someFunction3();
  } catch (BaseException &ex) {
    // catch块中的虚拟函数能够如我们所愿那样工作了：调用的函数是我们重新定义过的派生类函数。
    ex.print();
  }
}

// 如果你通过引用捕获异常（catch by reference），你就能避开上述所有问题：
//  不会为是否删除异常对象而烦恼；
//  能够避开slicing异常对象；
//  能够捕获标准异常类型；
//  减少异常对象需要被拷贝的数目。
// 所以你还在等什么？通过引用捕获异常吧（Catch exceptions by reference）！

int main() {
  doSomething();
  // MyException
  // throw MyException
  // catch MyException

  doSomething3();
  // DerivedException
  // BaseException::print

  doSomething4();
  // DerivedException
  // DerivedException::print

  return 0;
}