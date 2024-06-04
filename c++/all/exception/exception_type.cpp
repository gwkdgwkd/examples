#include <iostream>

namespace n1 {
// try{
//   可能抛出异常的语句
// }catch(exceptionType variable){
//   处理异常的语句
// }
// exceptionType是异常类型，它指明了当前的catch可以处理什么类型的异常；
// variable是一个变量，用来接收异常信息。
// 当程序抛出异常时，会创建一份数据，这份数据包含了错误信息，
// 可以根据这些信息来判断出了什么问题，接下来怎么处理。
// 异常既然是一份数据，那么就应该有数据类型。

// C++规定，异常类型可以是int、char、float、bool等基本类型，
// 也可以是指针、数组、字符串、结构体、类等聚合类型。
// C++语言本身以及标准库中的函数抛出的异常，都是exception类或其子类的异常。
// 也就是说，抛出异常时，会创建一个exception类或其子类的对象，称为标准异常。

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

struct A {
  std::string what() { return "A"; }
};
class C {
 public:
  std::string what() { return "C"; }
};
void func(int i) {
  int a[3] = {0, 1, 2};
  try {
    switch (i) {
      case 0:
        throw 5;
        break;
      case 1:
        throw 3.4;
        break;
      case 2:
        throw 3.4f;
        break;
      case 3:
        throw "nihao";
        break;
      case 4:
        throw std::string("nihao");
        break;
      case 5:
        throw 'a';
        break;
      case 6:
        throw false;
        break;
      case 7:
        throw a;
        break;
      case 8:
        throw A();
        break;
      case 9:
        throw C();
        break;
      case 10:
        throw std::out_of_range("out_of_range");
        break;
      case 11:
        throw std::exception();
        break;
    }

  } catch (int i) {
    std::cout << "i:" << i << std::endl;
  } catch (double d) {
    std::cout << "d:" << d << std::endl;
  } catch (float f) {
    std::cout << "f:" << f << std::endl;
  } catch (std::string s) {
    std::cout << "string:" << s << std::endl;
  } catch (const char* s) {
    std::cout << "char*:" << s << std::endl;
  } catch (char c) {
    std::cout << "c:" << c << std::endl;
  } catch (bool b) {
    std::cout << std::boolalpha << "b:" << b << std::endl;
  } catch (int a[]) {
    std::cout << "i[]:" << a[0] << "," << a[1] << "," << a[2] << std::endl;
  } catch (A a) {
    std::cout << "struct:" << a.what() << std::endl;
  } catch (C c) {
    std::cout << "class:" << c.what() << std::endl;
  } catch (std::out_of_range e) {
    std::cout << "o:" << e.what() << std::endl;
  } catch (std::exception e) {
    std::cout << "e:" << e.what() << std::endl;
  }
}  // namespace n1

void func() {
  func(0);   // i:5
  func(1);   // d:3.4
  func(2);   // f:3.4
  func(3);   // char*:nihao
  func(4);   // string:nihao
  func(5);   // c:a
  func(6);   // b:false
  func(7);   // i[]:0,1,2
  func(8);   // struct:A
  func(9);   // class:C
  func(10);  // o:out_of_range
  func(11);  // e:std::exception
}
}  // namespace n1

namespace n2 {
// exceptionType variable和函数的形参非常类似，当异常发生后，
// 会将异常数据传递给variable这个变量，这和函数传参的过程类似。
// 可以将catch看做一个没有返回值的函数，当异常发生后catch会被调用，
// 并且会接收实参（异常数据）。

// 但是catch和真正的函数调用又有区别：
// 1.函数调用，形参和实参的类型必须要匹配或者可以自动转换，否则就报错；
// 2.而对于catch，异常是在运行阶段产生的，它可以是任何类型，
//   没法提前预测，所以不能在编译阶段判断类型是否正确，只能等到程序运行后，
//   真的抛出异常了，再将异常类型和catch能处理的类型进行匹配，
//   匹配成功的话就调用当前的catch，否则就忽略当前的catch。
// 总起来说，catch和真正的函数调用相比，多了运行时将实参和形参匹配的过程。

void func1() {
  // 如果不希望catch处理异常数据，也可以将variable省略掉，
  // 这样只会将异常类型和catch所能处理的类型进行匹配，不会传递异常数据了：
  try {
    throw 5;
  } catch (int) {
    std::cout << "catch int exception" << std::endl;
  }

  // catch int exception
}

void func2() {
  // 只有跟exceptionType类型匹配的异常数据才会被传递给variable，
  // 否则catch不会接收这份异常数据，也不会执行catch块中的语句：
  try {
    throw "nihao";
  } catch (int i) {
    std::cout << "catch int exception" << std::endl;
  }
  // terminate called after throwing an instance of 'char const*'
}

void func() {
  func1();
  func2();
}
}  // namespace n2

namespace n3 {
// 发生异常时，程序从上到下将异常类型和catch接收类型逐个匹配。
// 一旦找到类型匹配的catch就停止，并将异常交给当前的catch处理（其他的不会执行）。
// 如果最终也没有找到匹配的catch，就只能交给系统处理，将终止程序。

// catch在匹配过程中的类型转换：
// C/C++中存在多种多样的类型转换，普通函数（非模板函数）发生调用时，
// 如果实参和形参的类型不是严格匹配，那么会将实参的类型进行适当的转换，
// 以适应形参的类型，包括：
// 1.算数转换：int转换为float，char转换为int，double转换为int等；
// 2.向上转型：也就是派生类向基类的转换；
// 3.const转换：将非const类型转换为const类型，将char*转换为const char*；
// 4.数组或函数指针转换：如果函数形参不是引用类型，
//   那么数组名会转换为数组指针，函数名也会转换为函数指针；
// 5.用户自定义的类型转换。
// catch在匹配异常类型的过程中，也会进行类型转换，但是受到了更多的限制，
// 仅能进行向上转型、const转换和数组或函数指针转换，其他的都不能应用于catch。

class Base {};
class Derived : public Base {};
void func1() {  // 向上转型
  try {
    throw Derived();  // 抛出异常，实际上是创建Derived类型的匿名对象
    std::cout << "This statement will not be executed." << std::endl;
  } catch (Base) {
    std::cout << "Exception type: Base" << std::endl;
  } catch (Derived) {
    // warning：exception of type ‘n3::Derived’ will be caught
    std::cout << "Exception type: Derived" << std::endl;
  }

  // Exception type: Base
}

void func2() {  // const转换
  try {
    char* p = "nihao";
    throw p;
  } catch (const char*) {
    std::cout << "Exception type: const cahr *" << std::endl;
  }

  // Exception type: const cahr *
}

void func3() {  // 数组指针转换和const转换
  int nums[] = {1, 2, 3};
  try {
    throw nums;
  } catch (const int*) {
    // 没有严格匹配的类型：先把int[3]先转换为int*，再转换为const int*。
    std::cout << "Exception type: const int *" << std::endl;
  }

  // Exception type: const int *
}

void func() { std::cout << "Exception type: func" << std::endl; }
void func4() {  // 函数指针转换
  try {
    throw func;
  } catch (void (*p)()) {
    p();
  }

  // Exception type: func
}

void func5() {  // catch不进行算术转换
  try {
    throw 3;
  } catch (float f) {
    std::cout << "Exception type: f" << std::endl;
  }

  // terminate called after throwing an instance of 'int'
}
}  // namespace n3

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 6]" << std::endl;
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
      n3::func1();
      break;
    case 3:
      n3::func2();
      break;
    case 4:
      n3::func3();
      break;
    case 5:
      n3::func4();
      break;
    case 6:
      n3::func5();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}