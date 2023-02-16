#include <iostream>
#include <string>

// 程序运行时常会碰到一些错误，例如除数为0、年龄为负数、数组下标越界等，
// 这些错误如果不能发现并加以处理，很可能会导致程序崩溃。
// C++异常处理机制就可以捕获并处理这些错误，
// 然后可以让程序沿着一条不会出错的路径继续执行，
// 或者不得不结束程序，但在结束前可以做一些必要的工作，
// 例如将内存中的数据写入文件、关闭打开的文件、释放分配的内存等。
// C++异常处理机制会涉及try、catch、throw三个关键字。

// 开发程序是一项烧脑的工作，程序员不但要经过长期的知识学习和思维训练，
// 还要做到一丝不苟，注意每一个细节和边界，即使这样，也不能防止程序出错。
// 程序的错误大致可以分为三种：
// 1.语法错误在编译和链接阶段就能发现，
//   只有100%符合语法规则的代码才能生成可执行程序。
//   语法错误是最容易发现、最容易定位、最容易排除的错误，
//   程序员最不需要担心的就是这种错误；
// 2.逻辑错误是说编写的代码思路有问题，不能够达到最终的目标，
//   这种错误可以通过调试来解决；
// 3.运行时错误是指程序在运行期间发生的错误，
//   例如除数为0、内存分配失败、数组越界、文件不存在等。
//   C++异常（Exception）机制就是为解决运行时错误而引入的。
// 运行时错误如果放任不管，系统就会执行默认的操作，
// 终止程序运行，也就是常说的程序崩溃（Crash）。
// C++提供了异常（Exception）机制，能够捕获运行时错误，
// 给程序一次起死回生的机会，或者至少告诉用户发生了什么再终止程序。
// 抛出异常就是报告运行时错误，程序员可以根据错误信息来进一步处理。

// 可以借助C++异常机制来捕获上面的异常，避免程序崩溃。
// 捕获异常的语法为：
// try {
//   可能抛出异常的语句
// } catch (exceptionType variable) {
//   处理异常的语句
// }
// try和catch都是C++中的关键字，后跟语句块，不能省略{}。
// try中包含可能会抛出异常的语句，一旦有异常抛出就会被catch捕获。
// try只是检测语句块有没有异常，如果没有发生异常，它就检测不到。
// 如果try语句块没有检测到异常，那么就不会执行catch中的语句。
// catch后面的exceptionType variable，
// 指明了当前catch可以处理的异常类型以及具体的出错信息。
// 换句话说，发生异常时必须将异常明确地抛出，try才能检测到；
// 如果不抛出来，即使有异常try也检测不到。
// 所谓抛出异常，就是明确地告诉程序发生了什么错误。
// 异常的处理流程：抛出（throw）--> 检测（try） --> 捕获（catch）

namespace n1 {
// 异常可以发生在当前的try块中，也可以发生在try块所调用的某个函数中，
// 或者是所调用的函数又调用了另外的一个函数，这个另外的函数中发生了异常，
// 这些异常，都可以被try检测到。
// throw关键字用来抛出一个异常，这个异常会被try检测到，进而被catch捕获。
// 发生异常后，程序的执行流会沿着函数的调用链往前回退，直到遇见try才停止。
// 回退过程中，调用链中剩下的代码都会被跳过，没有执行的机会了。
void func1(bool b) {
  if (b) {
    throw "func1 Unknown Exception";  // 抛出异常
  }
  std::cout << "[1]This statement will not be executed." << std::endl;
}

void func2(bool b) {
  func1(b);
  if (b) {
    throw "func2 Unknown Exception";  // 抛出异常
  }
  std::cout << "[2]This statement will not be executed." << std::endl;
}

void func3(bool b) {
  try {
    func2(b);
    std::cout << "[3]This statement will not be executed." << std::endl;
  } catch (const char*& e) {
    std::cout << e << std::endl;
  }
}

void func() {
  func3(false);
  // [1]This statement will not be executed.
  // [2]This statement will not be executed.
  // [3]This statement will not be executed.

  func3(true);
  // func1 Unknown Exception
}
}  // namespace n1

namespace n2 {
// C++语言本身或者标准库抛出的异常都是exception的子类，称为标准异常。
// try {
//   可能抛出异常的语句
// } catch(exception &e) {
//   处理异常的语句
// }
// 之所以使用引用，是为了提高效率，如果不使用引用，
// 就要经历一次对象拷贝（要调用拷贝构造函数）的过程。

class A {
 public:
  A() { std::cout << "A" << std::endl; }
  A(const A&) { std::cout << "A copy" << std::endl; }
};
void func1() {
  try {
    throw A();
  } catch (A) {
    std::cout << "catch exception A" << std::endl;
  }

  // A
  // A copy
  // catch exception A
}
void func2() {
  try {
    throw A();
  } catch (A&) {
    std::cout << "catch exception A&" << std::endl;
  }

  // A
  // catch exception A&
}

// what()函数返回一个能识别异常的字符串，正如它的名字what一样，
// 可以粗略地告诉这是什么异常，不过C++标准并没有规定这个字符串的格式，
// 各个编译器的实现也不同，所以what()的返回值仅供参考。
void func3() {
  try {
    throw std::exception();
  } catch (std::exception e) {
    std::cout << e.what() << std::endl;
  }

  // std::exception
}

// exception类的继承层次：
// logic_error：逻辑错误。
//  length_error：试图生成一个超出该类型最大长度的对象时抛出该异常，
//                例如vector的resize操作。
//  domain_error：参数的值域错误，主要用在数学函数中，
//                例如使用一个负值调用只能操作非负数的函数。
//  out_of_range：超出有效范围。
//  invalid_argument：参数不合适，在标准库中，当利用string对象构造bitset时，
//                    而string中的字符不是0或1的时候，抛出该异常。
// runtime_error：运行时错误。
//  range_error：计算结果超出了有意义的值域范围。
//  overflow_error：算术计算上溢。
//  underflow_error：算术计算下溢。
// bad_alloc：使用new或new[]分配内存失败时抛出的异常。
// bad_typeid：使用typeid操作一个NULL指针，而且该指针是带有虚函数的类，
//             这时抛出bad_typeid异常。
// bad_cast：使用dynamic_cast转换失败时抛出的异常。
// ios_base::failure：io过程中出现的异常。
// bad_exception：这是个特殊的异常，如果函数的异常列表里声明了bad_exception异常，
//                当函数内部抛出了异常列表中没有的异常时，
//                如果调用的unexpected()函数中抛出了异常，
//                不论什么类型，都会被替换为bad_exception类型。

void func4() {
  // at()是string类的一个成员函数，它会根据下标来返回字符串的一个字符。
  // 与[]不同，at()会检查下标是否越界，如果越界就抛出一个异常；
  // 而[]不做检查，不管下标是多少都会照常访问。
  std::string str = "http://c.biancheng.net";
  try {
    char ch3 = str[100];
    std::cout << ch3 << std::endl;
  } catch (std::exception e) {
    std::cout << "[1]out of bound!" << std::endl;
  }
  try {
    char ch4 = str.at(100);
    std::cout << ch4 << std::endl;
  } catch (std::exception& e) {
    std::cout << "[2]out of bound!" << std::endl;
  }

  // [2]out of bound!
}
}  // namespace n2

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 4]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::func1();
      break;
    case 2:
      n2::func2();
      break;
    case 3:
      n2::func3();
      break;
    case 4:
      n2::func4();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}