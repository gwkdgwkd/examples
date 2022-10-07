#include <iostream>

// 枚举类型是C/C++中用户自定义的构造类型，它是由用户定义的若干枚举常量的集合。
// 枚举值对应整型数值，默认从0开始。
// 比如定义一个描述性别的枚举类型：
enum Gender { Male, Female };

// 传统枚举类型在设计上会存在以下几个问题：
// 1.同作用域同名枚举值会报重定义错误，
//   传统C++中枚举常量被暴漏在同一层作用域中，
//   如果同一作用域下有两个不同的枚举类型，
//   但含有同名的枚举常量也是会报编译错误：
//   enum Gender1 { Male1, Female };
//   error: redeclaration of ‘Female’
//   因为enum则是非强作用域类型，枚举常量可以直接访问，
//   这种访问方式与C++中具名的namespace、
//   class/struct以及union必须通过名字::成员名的访问方式大相径庭。

// 2.由于枚举类型被设计为常量数值的别名，
//   所以枚举常量总是可以被隐式转换为整型，且用户无法为枚举常量定义类型。
bool x = (Male == 1);
int t = Female;

// 3.枚举常量占用存储空间以及符号性不确定。
//   C++标准规定C++枚举所基于的基础类型是由编译器来具体实现，
//   这会导致枚举类型成员的基本类型存在不确定性问题，尤其是符号性问题。
enum A { A1 = 1, A2 = 2, ABig = 0xFFFFFFFFU };
enum B { B1 = 1, B2 = 2, BBig = 0xFFFFFFFFFUL };

// 为了解决以上传统枚举类型的缺陷：
// 1.非强作用域类型；
// 2.允许隐式转换为整型；
// 3.枚举常量占用存储空间以及符号性的不确定。
// C++11引入了强类型枚举解决了这些问题：
enum class Enumeration { VAL1, VAL2, VAL3 = 100, VAL4 };

// 强类型枚举具有如下几个优点：
// 1.强作用域，强类型枚举成员的名称不会被输出到其父作用域，
//   所以不同枚举类型定义同名枚举成员编译不会报重定义错误。
//   进而使用枚举类型的枚举成员时，必须指明所属范围，
//   比如Enum::VAL1，而单独的VAL1则不再具有意义；

// 2.转换限制，强类型枚举成员的值不可以与整型发生隐式相互转换。
//   比如Enumeration::VAL4==10;会触发编译错误；

// 3.可以指定底层类型。
//   强类型枚举默认的底层类型是int，但也可以显示地指定底层类型。
//   具体方法是在枚举名称后面加上:type，
//   其中type可以是除wchar_t以外的任何整型：
enum class Enumeration1 : char { VAL1, VAL2, VAL3 = 50, VAL4 };
enum class type7 : unsigned int { a = 0xFFFFFFFF };
//   既可以节省空间，又可以指定符号。

// 注意：
// 1.声明强类型枚举的时候，既可以使用关键字enum class，也可以使用enum struct。
//   事实上，enum struct与enum class在语法上没有任何区别。
enum struct XXX {};

// 2.由于强类型枚举是强类型作用域的，故匿名的enum class可能什么都做不了，
//   如下代码会报编译错误：error: anonymous scoped enum is not allowed
//   enum class { General, Light, Medium, Heavy } weapon;
//   当然对于匿名强类型枚举还是可以使用decltype来获得其类型并进而使用，
//   但是这样做可能违背强类型枚举进行匿名的初衷。

// 传统枚举类型为了配合C++11引入的强类型枚举，C++11对传统枚举类型进行了扩展：
// 1.底层的基本类型可以在枚举名称后加上:type，
//   其中type可以是除wchar_t以外的任何整型。
enum Type1 : char { Low1, Middle1, High1 };
// 2.C++11中，枚举类型的成员可以在枚举类型的作用域内有效，
//   其中Middle与Type::Middle都是合法的使用形式。
enum Type2 { Low, Middle, High };
Type2 type1 = Middle;
Type2 type2 = Type2::Middle;

int main() {
  std::cout << Male << std::endl;    // 0
  std::cout << Female << std::endl;  // 1

  // 在VC++（VS2017）中的输出结果分别是4、-1、4和-1，
  // 可见不同编译器对枚举常量的整型类型的宽度和符号有着不同的实现。
  // GNU C++会根据枚举数值的类型使用不同宽度和符号的整型，
  // VC++则始终以有符号int来表示枚举常量。
  std::cout << sizeof(A1) << " " << ABig << std::endl;  // 4 4294967295
  std::cout << sizeof(B1) << " " << BBig << std::endl;  // 8 68719476735

  std::cout << static_cast<int>(Enumeration::VAL3) << std::endl;   // 100
  std::cout << static_cast<int>(Enumeration1::VAL3) << std::endl;  // 50

  // error: ‘VAL1’ was not declared in this scope
  // cout << VAL1 << endl;

  // no match for ‘operator==’ (operand types are ‘Enumeration’ and ‘int’)
  // Enumeration::VAL1 == 10;

  std::cout << sizeof(Enumeration::VAL3) << std::endl;   // 4
  std::cout << sizeof(Enumeration1::VAL3) << std::endl;  // 1

  return 0;
}