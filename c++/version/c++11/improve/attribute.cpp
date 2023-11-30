#include <iostream>

// 随着C++的演化和编译器的发展，常会发现标准提供的能力不能完全满足要求。
// 于是编译器厂商或组织为了满足客户的需求，设计出一系列的语言扩展语法。
// 这些扩展语法并不存在于C++/C标准中，却有可能拥有较多的用户。
// 扩展语法中比较常见的就是属性，属性是对语言中的实体对象，比如函数、变量、类型等，
// 附加一些的额外注解信息，其用来实现一些语言及非语言层面的功能，或是实现优化代码等的一种手段。
// 不同编译器有不同的属性语法，比如对于g++，属性是通过GNU的关键字__attribute__来声明的。
// 程序员只需要简单地声明：__attribute__((attribute-list))
// 即可为程序中的函数、变量和类型设定一些额外信息，以便编译器可以进行错误检查和性能优化等。
extern int area(int n) __attribute__((const));
// const属性告诉编译器函数返回值只依赖于输入，不会改变任何函数外的数据，因此没有任何副作用。
// 编译器可以对area函数进行优化处理，area(3)的值只需要计算一次，
// 编译之后可以将area(3)视为循环中的常量而只使用其计算结果，从而大大提高了程序的执行性能。

// 在GNU对C/C++的扩展中可以看到很多不同的attribute属性：
// 常见的如format、noreturn、const和aligned等，具体含义和用法可以参考GNU的在线文档。

// 在Windows平台上，使用另外一种关键字__declspec，是微软用于指定存储类型的扩展属性关键字，
// 只要简单地在声明变量时加上即可设定额外的功能：__declspec(extended-decl-modifier)
// 以对齐方式为例，在C++11之前，微软平台可以使用__declspec(align(x))来控制变量的对齐方式：
// __declspec(align(32)) struct Struct32 {
//   int i;
//   double d;
// };

namespace n1 {
// C++11语言中的通用属性使用了左右双中括号的形式：[[attribute-list]]，这样设计得好处是：
// 1.既不会消除语言添加或者重载关键字的能力；
// 2.又不会占用用户空间的关键字的名字空间。
// 语法上，C++11的通用属性可以作用于类型、变量、名称、代码块等。
// 对于作用于声明的通用属性，既可以写在声明的起始处，也可以写在声明之后。
// 而对于作用于整个语句的通用属性，则应该写在语句起始处。
// 而出现在以上两种规则描述的位置之外的通用属性，作用于哪个实体跟编译器具体的实现有关。

// [[attr1]] void func[[attr2]]();
// [[attr1]]出现在函数定义之前，而[[attr2]]则位于函数名称之后，
// 根据定义，[[attr1]]和[[attr2]]均可以作用于函数[func]。

// [[attr1]] int array[[attr2]][10];
// [[attr1]]和[[attr2]]均可以作用于数组array。

// [[attr1]] class C[[attr2]]{}[[attr3]] c1[[attr4]], c2[[attr5]];
// 声明了类C及其类型的变量c1和c2，一共有5个不同的属性。
// 按照C++11的定义：
// [[attr1]]和[[attr4]]会作用于c1；
// [[attr1]]和[[attr5]]会作用于c2；
// [[attr2]]出现在声明之后，仅作用于类C；
// [[attr3]]所作用的对象则跟具体实现有关。

// [[attr1]] L1:
// switch(value){
// [[attr2]] case1: //do something...
// [[attr3]] case2: //do something...
// [[attr4]] break;
// [[attr5]] default: //do something...
// }
// [[attr6]] goto L1;
// [[attr1]]作用于标签L1，
// [[attr2]]和[[attr3]]作用于case 1和case 2表达式，
// [[attr4]]作用于break,
// [[attr5]]作用于default表达式，
// [[attr6]]作用于goto语句。

// [[attr1]] for(int i=0;i<top;i++){
// }
// [[attr2]] return top;

// [[attr1]] int func([[attr2]] int i,[[attr3]] int j) {
// [[attr4]] return i+j;
// }
// [[attr1]]作用于函数func,
// [[attr2]]和[[attr3]]分别作用于整型参数i和j,
// [[attr4]]作用于return语句。

// 在现有C++11标准中，只预定义了两个通用属性，[[noreturn]]和[[carries_dependency]]。
// [[noreturn]]是用于标识不会返回的函数的，不会返回和没有返回值的（void）函数的区别：
// 1.没有返回值的void函数在调用完成后，调用者会接着执行函数后的代码；
// 2.而不会返回的函数在被调用完成后，后续代码不会再被执行。
// [[noreturn]]主要用于标识那些不会将控制流返回给原调用函数的函数：
// 1.有终止应用程序语句的函数；
// 2.有无限循环语句的函数；
// 3.有异常抛出的函数等。
// 通过这个属性，开发人员可以告知编译器某些函数不会将控制流返回给调用函数，
// 这能帮助编译器产生更好的警告信息，同时编译器也可以做更多的诸如死代码消除、
// 免除为函数调用者保存一些特定寄存器等代码优化工作。
void DoSomething1() { std::cout << "DoSomething1" << std::endl; };
void DoSomething2() { std::cout << "DoSomething2" << std::endl; };
[[noreturn]] void ThrowAway() {
  throw "expection";  // 控制流跳转到异常处理
}
void func1() {
  DoSomething1();
  ThrowAway();
  DoSomething2();  // 该函数不可到达
}

// 在C++11的标准库中，都能看到形如：
[[noreturn]] void abort(void) noexcept;
// abort总是会导致程序运行的停止，甚至连自动变量的析构，
// 以及本该在atexit()时调用的函数全都不调用就直接退出了。
// 因此声明为[[noreturn]]是有利于编译器优化的。

// 尽量不要对可能会有返回值的函数使用[[noreturn]]:
[[noreturn]] void func2(int i) {
  // warning: ‘noreturn’ function does return
  if (i < 0)
    throw "negative";
  else if (i > 0)
    throw "positive";
  // else
  // 当参数i的值为0时，该函数行为不可估计。
}

// 另外一个通用属性[[carries_dependency]]则跟并行情况下的编译器优化有关。
// 主要是为了解决弱内存模型平台上使用memory_order_consume内存顺序枚举问题。
// 该通用属性既可以标识函数参数，又可以标识函数的返回值。

}  // namespace n1

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func1();
      break;
    case 1:
      n1::func2(-1);
      break;
    case 2:
      n1::func2(1);
      break;
    case 3:
      n1::func2(0);
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}