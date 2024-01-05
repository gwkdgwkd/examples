#include <iostream>
#include <vector>

// 条款4：知道如何查看类型推导

// 对于类型推导结果的查看工具的选择和在软件开发过程中的相关信息有关，有三种可能：
// 1.编写代码时；
// 2.编译的时候；
// 3.运行的时候得到类型推导的信息。

namespace n1 {
// 在IDE里，光标悬停在实体之上，常常可以显示出变量、参数、函数等的类型。
// 对于简单的类型如int，信息是正确的，设计到更加复杂的类型时，有时信息没有帮助。
}  // namespace n1

namespace n2 {
// 让编译器展示类型的办法是故意制造编译错误，错误输出会描述错误相关的类型。

template <typename T>
class TD;
void func() {
  int i;
  // TD<decltype(i)> iType;  // aggregate ‘n2::TD<int> iType’ has incomplete ...

  float f;
  // TD<decltype(f)> fType;  // aggregate ‘n2::TD<float> fType’ has ...
}
}  // namespace n2

struct Widget {};
std::vector<Widget> createVec() { return std::vector<Widget>{Widget()}; }
namespace n3 {
// 使用typeid和std::type_info::name来获取运行时的类型。
// typeid得到std::type_info的对象，成员函数name返回C-style的字符串，表示类型名。
// std::type_info::name并不确定返回有意义的东西，但实际是有帮助的，
// i意味这int，PK意味这pointer to const。
// 所有的编译器都支持c++filt工具，能够解析name函数返回的类型名称。

template <typename T>
void fun(const T& param) {
  std::cout << "T = " << typeid(T).name() << std::endl;
  std::cout << "param = " << typeid(param).name() << std::endl;
}

void func() {
  int x;
  std::cout << typeid(x).name() << std::endl;  // i

  float f;
  std::cout << typeid(f).name() << std::endl;  // f

  const auto vw = createVec();
  fun(&vw[0]);

  // T = PKN2n36WidgetE
  // param = PKN2n36WidgetE

  // c++filt -t PKN2n36WidgetE
  // n3::Widget const*

  // param和T的类型是一致的，所以std::type_info::name的结果并不可靠。
  // 因为std::type_info::name会被当做它们被传给模板函数的时候按值传递的参数，
  // 这就意味着如果类型是引用，那么引用会被忽略，如果忽略引用后，
  // 还有CV特性，那么CV特性也会被忽略，所以：
  // param真实的类型是const Widget* const&，但结果却是const Widget*。
  // 同理，由IDE编辑器显示的类型信息也不准确，或者说至少并不可信。
}
}  // namespace n3

namespace n4 {
// #include <boost/type_index.hpp>
template <typename T>
void fun(const T& param) {
  // using boost::typeindex::type_id_with_cvr;
  // std::cout << "T = " << type_id_with_cvr(T).pretty_name() << std::endl;
  // std::cout << "param=" << type_id_with_cvr(param).pretty_name() <<std::endl;
}

void func() {
  // fun(&vw[0]);
}
}  // namespace n4

// 要记住的东西：
// 1.类型推导的结果可以通过IDE编辑器，
//   编译器错误输出信息和Boost TypeIndex库的结果中得到；
// 2.一些工具的结果不一定有帮助也不一定准确，所以对C++的类型推导法则加以理解很有必要。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n2::func();
      break;
    case 1:
      n3::func();
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