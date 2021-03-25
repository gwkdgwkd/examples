#include <iostream>

using namespace std;

// 程序运行时常会碰到一些错误，例如除数为0、年龄为负数、数组下标越界等，这些错误如果不能发现并加以处理，很可能会导致程序崩溃。
// C++异常处理机制就可以捕获并处理这些错误，然后可以让程序沿着一条不会出错的路径继续执行，或者不得不结束程序，但在结束前可以做一些必要的工作，例如将内存中的
// 数据写入文件、关闭打开的文件、释放分配的内存等。
// C++异常处理机制会涉及try、catch、throw三个关键字。

// 开发程序是一项“烧脑”的工作，程序员不但要经过长期的知识学习和思维训练，还要做到一丝不苟，注意每一个细节和边界。即使这样，也不能防止程序出错。
// 程序的错误大致可以分为三种，分别是语法错误、逻辑错误和运行时错误：
//  1) 语法错误在编译和链接阶段就能发现，只有100%符合语法规则的代码才能生成可执行程序。
//     语法错误是最容易发现、最容易定位、最容易排除的错误，程序员最不需要担心的就是这种错误。
//  2) 逻辑错误是说我们编写的代码思路有问题，不能够达到最终的目标，这种错误可以通过调试来解决。
//  3) 运行时错误是指程序在运行期间发生的错误，例如除数为0、内存分配失败、数组越界、文件不存在等。C++异常（Exception）机制就是为解决运行时错误而引入的。
// 运行时错误如果放任不管，系统就会执行默认的操作，终止程序运行，也就是我们常说的程序崩溃（Crash）。C++提供了异常（Exception）机制，让我们能够捕获运行时错误，
// 给程序一次“起死回生”的机会，或者至少告诉用户发生了什么再终止程序。
// 所谓抛出异常，就是报告一个运行时错误，程序员可以根据错误信息来进一步处理。
// 可以借助C++异常机制来捕获上面的异常，避免程序崩溃。捕获异常的语法为：
// try{
//     // 可能抛出异常的语句
// }catch(exceptionType variable){
//     // 处理异常的语句
// }
// try和catch都是C++中的关键字，后跟语句块，不能省略{}。try中包含可能会抛出异常的语句，一旦有异常抛出就会被后面的catch捕获。
// 从try的意思可以看出，它只是“检测”语句块有没有异常，如果没有发生异常，它就“检测”不到。catch是“抓住”的意思，用来捕获并处理try检测到的异常；
// 如果try语句块没有检测到异常（没有异常抛出），那么就不会执行catch中的语句。
// 这就好比，catch告诉try：你去检测一下程序有没有错误，有错误的话就告诉我，我来处理，没有的话就不要理我！
// catch关键字后面的exceptionType variable指明了当前catch可以处理的异常类型，以及具体的出错信息。
// 换句话说，发生异常时必须将异常明确地抛出，try才能检测到；如果不抛出来，即使有异常try也检测不到。所谓抛出异常，就是明确地告诉程序发生了什么错误。
// 异常的处理流程：抛出（Throw）--> 检测（Try） --> 捕获（Catch）
// 异常可以发生在当前的try块中，也可以发生在try块所调用的某个函数中，或者是所调用的函数又调用了另外的一个函数，这个另外的函数中发生了异常。这些异常，都可以被try检测到。
// throw关键字用来抛出一个异常，这个异常会被try检测到，进而被catch捕获。
// 发生异常后，程序的执行流会沿着函数的调用链往前回退，直到遇见try才停止。在这个回退过程中，调用链中剩下的代码（所有函数中未被执行的代码）都会被跳过，没有执行的机会了。

void func() {
  throw "func Unknown Exception";  // 抛出异常
  cout << "[1]This statement will not be executed." << endl;
}

void func_inner() {
  throw "func_inner Unknown Exception";  //抛出异常
  cout << "[1]This statement will not be executed." << endl;
}
void func_outer() {
  func_inner();
  cout << "[2]This statement will not be executed." << endl;
}

int main() {
  string str = "http://c.biancheng.net";
  char ch1 = str[100];  // 下标越界，ch1为垃圾值
  cout << ch1 << endl;
  // char ch2 = str.at(100);  // 下标越界，抛出异常
  // cout << ch2 << endl;
  // at()是string类的一个成员函数，它会根据下标来返回字符串的一个字符。与[]不同，at()会检查下标是否越界，如果越界就抛出一个异常；而[]不做检查，不管下标是多少都会照常访问。

  try {
    char ch3 = str[100];
    cout << ch3 << endl;
  } catch (exception e) {
    cout << "[1]out of bound!" << endl;
  }
  try {
    char ch4 = str.at(100);
    cout << ch4 << endl;
  } catch (exception &e) {  // exception类位于<exception>头文件中
    cout << "[2]out of bound!" << endl;  // [2]out of bound!
  }

  try {
    throw "Unknown Exception";  // 抛出异常
    cout << "This statement will not be executed." << endl;
  } catch (const char *&e) {
    cout << e << endl;
  }

  try {
    func();
    cout << "[2]This statement will not be executed." << endl;
  } catch (const char *&e) {
    cout << e << endl;
  }

  try {
    func_outer();
    cout << "[3]This statement will not be executed." << endl;
  } catch (const char *&e) {
    cout << e << endl;
  }

  return 0;
}