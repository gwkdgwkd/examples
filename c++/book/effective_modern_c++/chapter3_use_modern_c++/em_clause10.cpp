#include <iostream>
#include <string>
#include <tuple>

// 条款10：优先使用作用域限制的enums而不是无作用域的enum

namespace n1 {
namespace test1 {
// 一般而言，花括号里声明的变量在括号外不可见，但对于C++98的enum却可见，
// 枚举元素和枚举类型同属于一个作用域空间，意味作用域中不能在有同样的名字：
enum Color1 { black, white, red };  // 官方称作无作用域的enum
// enum Color2 { blue, white };  // 报错

void func() {
  // auto white = false;  // 报错
}
}  // namespace test1

namespace test2 {
// 限制作用域的enum可以减少命名空间的污染：
enum class Color1 { black, white, red };  // 有作用域的enum
enum class Color2 { blue, white };        // 枚举类

void func() {
  auto white = false;

  // Color1 c1 = white;  // 报错
  Color1 c2 = Color1::white;
  auto c3 = Color1::white;
}
}  // namespace test2

void func() {
  test1::func();
  test2::func();
}
}  // namespace n1

namespace n2 {
// 限制作用域的enum还有一个令人不可抗拒的优势，它们的枚举元素可以是更丰富的类型。

namespace test1 {
enum Color { black, white, red };

void func() {
  Color c = red;
  if (c < 14.5) {
    std::cout << "c < 14.5" << std::endl;
  }

  // c < 14.5
}
}  // namespace test1

namespace test2 {
enum class Color { black, white, red };

void func() {
  Color c = Color::red;
  // if (c < 14.5) {  // 报错
  if (static_cast<int>(c) < 14.5) {  // 类型强制转换
    std::cout << "c < 14.5" << std::endl;
  }

  // c < 14.5
}
}  // namespace test2
void func() {
  test1::func();
  test2::func();
}
}  // namespace n2

namespace n3 {
// 相较于无定义域的enum，有定义域的enum还有第三个优势，可以提前声明。

namespace test1 {
// 编译器通常向为枚举选择可以充分表示枚举元素取值范围但有占用内存最小的潜在类型，
// 目的是充分利用内存，某些情况下，为了代码速度的优化，可以牺牲内存大小，
// 此时可能不会选择占用内存最小的可允许的潜在类型，但编译器依然希望优化内存大小。
// 为了实现这种功能，C++98仅仅支持枚举类型的定义，而枚举类型的声明是不被允许的。
// 这样可以保证在枚举类型被使用之前，编译器已经给每个枚举类型选择了潜在类型。

// enum Color1;  // 报错
enum class Color2;
enum Color3 : char;
}  // namespace test1

namespace test2 {
// 不能事先声明枚举类型有几个不足，最引人注目的是会增加编译依赖性。

enum Status {
  good = 0,
  failed = 1,
  imcomplete = 100,
  corrupt = 200,
  audited = 500,  // 新增
  indeterminate = 0xFFFFFFFF
};
// 这个枚举在整个系统中都会被使用，所以被放到头文件中，如果需要增加新的状态，
// 就算子系统，甚至只有一个函数用到这个新的枚举元素，可能导致整个系统都要重新编译。
// C++11中，这种情况被消除了，使用前置声明，这样使用到新增加枚举元素的模块，
// 需要重新编译，没有使用到新增加的枚举元素的其他模块，不用重新编译。

// 但编译器需要在枚举定义之前指定它的大小，枚举类是怎么做的可以前置声明的？
// 枚举类的潜在类型是已知的，默认是int，也可以手动指定，
// 而枚举没有潜在类型，所以不能前置声明，但是如果手动指定类型，那么就可以前置声明。
// 指定潜在类型也可以放在枚举体的定义处。

}  // namespace test2
}  // namespace n3

namespace n4 {
// 枚举比枚举类有用的场景：

using UserInfo = std::tuple<std::string,   // 姓名
                            std::string,   // 电子邮件
                            std::size_t>;  // 影响力
namespace test1 {
void func() {
  UserInfo uInfo;
  auto val = std::get<1>(uInfo);  // 虽然有注释，但还是不直观
}
}  // namespace test1

namespace test2 {
enum UserInfoFields { uiName, uiEmail, uiReputation };
void func() {
  UserInfo uInfo;
  auto val = std::get<uiEmail>(uInfo);  // 枚举可以隐式类型转换，直观
}
}  // namespace test2

namespace test3 {
enum class UserInfoFields { uiName, uiEmail, uiReputation };
void func1() {
  UserInfo uInfo;
  auto val = std::get<static_cast<std::size_t>(UserInfoFields::uiEmail)>(uInfo);
}

// 写一个枚举元素为参数返回对应的std::size_t的函数来减少冗余：
// 1.std::get是模板，所以函数必须在编译阶段就知道结构，必须为constexpr函数；
// 2.需要对任何类型的枚举类都有效，所有必须是constexpr函数模板；
// 3.为了一般化，不能返回std::size_t，需要用std::underlying_type返回枚举类的潜在类型；
// 4.最后需要声明为noexcept，因为永远不会触发异常。
template <typename E>
constexpr typename std::underlying_type<E>::type toUType1(E e) noexcept {
  return static_cast<typename std::underlying_type<E>::type>(e);
}

#if __cplusplus == 201402L
// 在C++14中，可以用std::underlying_type_t代替std::underlying_type<E>::type：
template <typename E>
constexpr std::underlying_type_t<E> toUType2(E e) noexcept {
  return static_cast<std::underlying_type_t<E>>(e);
}

// 在C++14中，还可以更加优雅的使用auto返回值类型：
template <typename E>
constexpr auto toUType3(E e) noexcept {
  return static_cast<std::underlying_type_t<E>>(e);
}
#endif

void func() {
  UserInfo uInfo;
  auto val1 = std::get<toUType1(UserInfoFields::uiEmail)>(uInfo);

#if __cplusplus == 201402L
  auto val2 = std::get<toUType2(UserInfoFields::uiEmail)>(uInfo);
  auto val3 = std::get<toUType3(UserInfoFields::uiEmail)>(uInfo);
#endif

  // 无论哪种形式，枚举类的方式依然比枚举的方式要复杂。
}
}  // namespace test3

void func() {
  test1::func();
  test2::func();
  test3::func();
}
}  // namespace n4

// 要记住的东西：
// 1.C++98风格的enum是没有作用域的enum；
// 2.有作用域的枚举的枚举元素仅仅对枚举内部可见，只能通过类型转换转换为其他类型；
// 3.有作用域和没有作用域的enum都支持指定潜在类型，有作用域的enum默认类型是int，
//   没有作用域的enum没有默认的潜在类型；
// 4.有作用的enum总可以前置声明，没有作用域的enum只有指定潜在类型时才能前置声明。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
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
      n4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}