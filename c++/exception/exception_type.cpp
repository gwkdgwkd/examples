#include <iostream>

using namespace std;

// try{
//   可能抛出异常的语句
// }catch(exceptionType variable){
//   处理异常的语句
// }
// exceptionType是异常类型，它指明了当前的catch可以处理什么类型的异常；variable是一个变量，用来接收异常信息。
// 当程序抛出异常时，会创建一份数据，这份数据包含了错误信息，程序员可以根据这些信息来判断到底出了什么问题，接下来怎么处理。
// 异常既然是一份数据，那么就应该有数据类型。C++规定，异常类型可以是int、char、float、bool等基本类型，也可以是指针、数组、字符串、结构体、类等聚合类型。
// C++语言本身以及标准库中的函数抛出的异常，都是exception类或其子类的异常。也就是说，抛出异常时，会创建一个exception类或其子类的对象。
// exceptionType variable和函数的形参非常类似，当异常发生后，会将异常数据传递给variable这个变量，这和函数传参的过程类似。当然，只有跟exceptionType
// 类型匹配的异常数据才会被传递给variable，否则catch不会接收这份异常数据，也不会执行catch块中的语句。换句话说，catch不会处理当前的异常。
// 可以将catch看做一个没有返回值的函数，当异常发生后catch会被调用，并且会接收实参（异常数据）。
// 但是catch和真正的函数调用又有区别：
//  1 真正的函数调用，形参和实参的类型必须要匹配，或者可以自动转换，否则在编译阶段就报错了。
//  2 而对于catch，异常是在运行阶段产生的，它可以是任何类型，没法提前预测，所以不能在编译阶段判断类型是否正确，只能等到程序运行后，真的抛出异常了，再将异常类
//    型和catch能处理的类型进行匹配，匹配成功的话就“调用”当前的catch，否则就忽略当前的catch。
// 总起来说，catch和真正的函数调用相比，多了一个「在运行阶段将实参和形参匹配」的过程。
// 另外需要注意的是，如果不希望catch处理异常数据，也可以将variable省略掉，也即写作：
// try{
//   可能抛出异常的语句
// }catch(exceptionType){
//   处理异常的语句
// }
// 这样只会将异常类型和catch所能处理的类型进行匹配，不会传递异常数据了。

// 一个try对应一个catch，这只是最简单的形式。其实，一个try后面可以跟多个catch：
// try{
//   可能抛出异常的语句
// } catch (exception_type_1 e){
//   处理异常的语句
// } catch (exception_type_2 e){
//   处理异常的语句
// } catch (exception_type_n e){
//   处理异常的语句
// }
// 当异常发生时，程序会按照从上到下的顺序，将异常类型和catch所能接收的类型逐个匹配。一旦找到类型匹配的catch就停止检索，并将异常交给当前的catch处理（其他的catch
// 不会被执行）。如果最终也没有找到匹配的catch，就只能交给系统处理，终止程序的运行。

// catch在匹配过程中的类型转换:
// C/C++中存在多种多样的类型转换，普通函数（非模板函数）发生调用时，如果实参和形参的类型不是严格匹配，那么会将实参的类型进行适当的转换，以适应形参的类型，包括：
//  算数转换：例如int转换为float，char转换为int，double转换为int等。
//  向上转型：也就是派生类向基类的转换。
//  const转换：也即将非const类型转换为const类型，例如将char *转换为const char *。
//  数组或函数指针转换：如果函数形参不是引用类型，那么数组名会转换为数组指针，函数名也会转换为函数指针。
//  用户自定的类型转换。
// catch在匹配异常类型的过程中，也会进行类型转换，但是受到了更多的限制，仅能进行「向上转型」、「const转换」和「数组或函数指针转换」，其他的都不能应用于catch。

class Base {};
class Derived : public Base {};

int main() {
  // 期望的是，异常被catch(Derived)捕获，但是从输出结果可以看出，异常提前被catch(Base)捕获了，这说明catch在匹配异常类型时发生了向上转型（Upcasting）。
  try {
    throw Derived();  // 抛出自己的异常类型，实际上是创建一个Derived类型的匿名对象
    cout << "This statement will not be executed." << endl;
  } catch (int) {
    cout << "Exception type: int" << endl;
  } catch (char *) {
    cout << "Exception type: cahr *" << endl;
  } catch (Base) {  // 匹配成功（向上转型）
    cout << "Exception type: Base" << endl;
  } catch (Derived) {
    cout << "Exception type: Derived" << endl;
  }
  // Exception type: Base

  // nums本来的类型是int [3]，但是catch中没有严格匹配的类型，所以先转换为int *，再转换为const int *。
  int nums[] = {1, 2, 3};
  try {
    throw nums;
    cout << "This statement will not be executed." << endl;
  } catch (const int *) {
    cout << "Exception type: const int *" << endl;
  }
  // Exception type: const int *

  return 0;
}