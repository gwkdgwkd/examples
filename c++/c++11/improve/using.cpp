#include <iostream>
#include <map>
#include <string>

namespace n1 {
// 在C++中可以通过typedef重定义一个类型:
typedef unsigned int uint_t;
// 被重定义的类型并不是一个新的类型，仅仅只是原有的类型取了一个新的名字。
// 因此，下面这样将不是合法的函数重载:
void func(unsigned int) {}
// void func(uint_t) {}  // error: redefinition

// 使用typedef重定义类型是很方便的，但它也有一些限制，比如，无法重定义一个模板。
// 想要实现下面的功能：
typedef std::map<std::string, int> map_int_t;
typedef std::map<std::string, std::string> map_str_t;

// 在C++98/03中:
template <typename Val>
struct str_map {
  typedef std::map<std::string, Val> type;
};

// 在C++11中终于出现了可以重定义一个模板的语法:
template <typename Val>
using str_map_t = std::map<std::string, Val>;
// 这里使用新的using别名语法定义了std::map的模板别名str_map_t。
// 比起前面使用外敷模板加typedef构建的str_map，它完全就像是一个新的map类模板，因此，简洁了很多。

void testN1() {
  str_map<int>::type mi = {{"one", 1}, {"two", 2}};
  str_map<float>::type mf = {{"one", 1.0}, {"two", 2.0}};
  str_map<std::string>::type ms = {{"one", "11"}, {"two", "22"}};

  str_map_t<int> mi1 = {{"one", 1}, {"two", 2}};
  str_map_t<float> mf1 = {{"one", 1.0}, {"two", 2.0}};
  str_map_t<std::string> ms1 = {{"one", "11"}, {"two", "22"}};
}
}  // namespace n1

namespace n2 {
// 实际上，using的别名语法覆盖了typedef的全部功能：

// 重定义unsigned int
typedef unsigned int uint_t;
using uint_t = unsigned int;
// 重定义std::map
typedef std::map<std::string, int> map_int_t;
using map_int_t = std::map<std::string, int>;
// 可以看到，在重定义普通类型上，两种使用方法的效果是等价的，唯一不同的是定义语法。

// typedef的定义方法和变量的声明类似：像声明一个变量一样，声明一个重定义类型，之后在声明之前加上typedef即可。
// 这种写法凸显了C/C++中的语法一致性，但有时却会增加代码的阅读难度。比如重定义一个函数指针时：
typedef void (*func_t)(int, int);

// 与之相比，using后面总是立即跟随新标识符，之后使用类似赋值的语法，把现有的类型赋给新类型：
using func_t = void (*)(int, int);
// 从上面的对比中可以发现，C++11的using别名语法比typedef更加清晰。
//  因为typedef的别名语法本质上类似一种解方程的思路；
//  而using语法通过赋值来定义别名，和我们平时的思考方式一致。
}  // namespace n2

namespace n3 {
// C++98/03定义模板别名：
template <typename T>
struct func_t1 {
  typedef void (*type)(T, T);
};

// C++11新using定义模板别名：
template <typename T>
using func_t2 = void (*)(T, T);

// 通过using定义模板别名的语法，只是在普通类型别名语法的基础上增加template的参数列表。
// 使用using可以轻松地创建一个新的模板别名，而不需要像C++98/03那样使用烦琐的外敷模板。
// func_t2既不是类模板也不是函数模板（函数模板实例化后是一个函数），而是一种新的模板形式，模板别名。
// 需要注意的是，using语法和typede一样，并不会创造新的类型。

// 通过using可以轻松定义任意类型的模板表达方式。比如下面这样：
template <typename T>
using type_t = T;  // type_t实例化后的类型和它的模板参数类型等价

void testN3() {
  func_t1<int>::type xx_1;
  func_t2<int> xx_2;
  // xx_2并不是一个由类模板实例化后的类，而是void(*)(int,int)的别名

  type_t<int> i;  // 等价为int
}
}  // namespace n3

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 0]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::testN1();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}
