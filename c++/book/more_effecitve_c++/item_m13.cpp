#include <exception>
#include <iostream>

// 通过引用（reference）捕获异常

// 当写一个catch子句时，必须确定让异常通过何种方式传递到catch子句里，
// 可以有三个选择，与给函数传递参数一样：
// 1.通过指针，by pointer；
// 2.通过传值，by value；
// 3.通过引用，by reference。

// 从throw处传递一个异常到catch子句是一个缓慢的过程，
// 在理论上这种方法的实现对于这个过程来说是效率最高的。
// 因为在传递异常信息时，只有采用通过指针抛出异常的方法才能够做到不拷贝对象。
class MyException : public std::exception {
 public:
  MyException() { std::cout << "MyException" << std::endl; }
  MyException(const MyException &) {
    std::cout << "MyException(const MyException &)" << std::endl;
  }
};

namespace n1 {
void someFunction() {
  static MyException ex;  // 异常对象
  std::cout << "throw MyException" << std::endl;
  throw &ex;  // 抛出一个指针，指向ex
}
void func() {
  try {
    someFunction();            // 抛出一个exception*
  } catch (MyException *ex) {  // 捕获exception*，没有对象被拷贝
    std::cout << "catch MyException" << std::endl;
  }

  // MyException
  // throw MyException
  // catch MyException
}
}  // namespace n1

namespace n2 {
// n1看上去很不错，但是实际情况却不是这样，为了能让程序正常运行，
// 定义异常对象时必须确保当程序离开抛出指针的函数后，对象还能继续生存。
// 全局与静态对象都能够做到这一点，但是程序员很容易忘记这个约束：
void someFunction1() {
  // 局部异常对象，当退出函数的生存空间时这个对象将被释放：
  MyException ex;
  // 处理这个异常的catch子句接收到的指针，其指向的对象已经不再存在。
  throw &ex;  // 抛出一个指针，指向已被释放的对象
}
// 另一种抛出指针的方法是建立一个堆对象（new heap object）：
void someFunction2() {
  // 抛出一个指针指向堆中建立的对象，希望操作符new不要再抛出异常：
  throw new MyException;
}
// 这避免了捕获一个指向已被释放对象的指针的问题，
// 但是catch子句的作者又面临一个令人头疼的问题：他们是否应该删除他们接受的指针？
// 如果是在堆中建立的异常对象，那必须删除它，否则会造成资源泄漏。
// 如果不是在堆中建立的异常对象，绝对不能删除它，否则行为将不可预测。
// 该如何做呢？这是不可能知道的。
// 一些被调用者可能会传递全局或静态对象的地址，另一些可能传递堆中建立的异常对象的地址。
// 通过指针捕获异常，将遇到一个哈姆雷特式的难题：
// 是删除还是不删除？这是一个难以回答的问题，所以最好避开它。
// 而且，通过指针捕获异常也不符合C++语言本身的规范。
// 四个标准的异常，都不是指向对象的指针，所以必须通过值或引用来捕获它们：
// bad_alloc，当operator new不能分配足够的内存时，被抛出；
// bad_cast，当dynamic_cast针对一个引用reference操作失败时，被抛出；
// bad_typeid，当dynamic_cast 对空指针进行操作时，被抛出；
// bad_exception，用于unexpected异常；

void func() {
  try {
    someFunction1();
  } catch (MyException *ex) {
    std::cout << "catch MyException" << std::endl;
  }
  // MyException
  // catch MyException

  try {
    someFunction2();
  } catch (MyException *ex) {
    std::cout << "catch MyException" << std::endl;
    delete ex;
  }
  // MyException
  // catch MyException
}
}  // namespace n2

// 通过值捕获异常可以解决n2的问题，例如异常对象删除的问题和使用标准异常类型的问题。
// 但是当它们被抛出时系统将对异常对象拷贝两次。
// 而且它会产生slicing problem，即派生类对象被做为基类对象捕获时，
// 那它的派生类行为就被切掉了（sliced off）。
// 这样的sliced对象实际上是一个基类对象：
// 它们没有派生类的数据成员，而且当本准备调用它们的虚拟函数时，
// 系统解析后调用的却是基类对象的函数。
// 当一个对象通过传值方式传递给函数，也会发生一样的情况。
class BaseException : public std::exception {
 public:
  virtual ~BaseException() { std::cout << "~BaseException" << std::endl; }
  virtual void print() { std::cout << "BaseException::print" << std::endl; }
};
class DerivedException : public BaseException {
 public:
  DerivedException() { std::cout << "DerivedException" << std::endl; }
  ~DerivedException() { std::cout << "~DerivedException" << std::endl; }
  virtual void print() { std::cout << "DerivedException::print" << std::endl; }
};
void someFunction() { throw DerivedException(); }

namespace n3 {
void func() {
  try {
    someFunction();
  } catch (BaseException ex) {  // 捕获所有标准异常类或它的派生类
    ex.print();                 // 调用BaseException::print()
  }

  // DerivedException
  // BaseException::print
  // ~BaseException
  // ~DerivedException
  // ~BaseException
}
}  // namespace n3

namespace n4 {
// 最后剩下方法就是通过引用捕获异常，通过引用捕获异常能避开上述所有问题：
// 1.不象通过指针捕获异常，不会有对象删除的问题而且也能捕获标准异常类型；
// 2.也不象通过值捕获异常，没有slicing problem，而且异常对象只被拷贝一次。
void func() {
  try {
    someFunction();
  } catch (BaseException &ex) {
    // catch块中的虚拟函数能够如所愿工作了，调用的函数是派生类函数：
    ex.print();
  }

  // DerivedException
  // DerivedException::print
  // ~DerivedException
  // ~BaseException
}
}  // namespace n4

// 如果通过引用捕获异常（catch by reference），就能避开所有问题：
// 1.不会为是否删除异常对象而烦恼（指针）；
// 2.能够避开slicing异常对象（传值）；
// 3.能够捕获标准异常类型（指针）；
// 4.减少异常对象需要被拷贝的数目（传值）。

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::func();
      break;
    case 2:
      n3::func();
      break;
    case 3:
      n4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}