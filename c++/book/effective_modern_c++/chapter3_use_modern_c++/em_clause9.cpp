#include <climits>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>

// 条款9：优先使用using而不是typedef

namespace n1 {
typedef void (*FP1)(int, const std::string&);
using FP2 = void (*)(int, const std::string&);

void func() {
  // 直接定义变量，每次都写这么长，太复杂了：
  std::unique_ptr<std::unordered_map<std::string, std::string>> m1;

  // typedef有浓厚的C++98气息：
  typedef std::unique_ptr<std::unordered_map<std::string, std::string>>
      UPtrMapSS1;
  UPtrMapSS1 m2;

  // C++11提供了声明别名，和typedef具有完全一样的意义：
  using UPtrMapSS2 =
      std::unique_ptr<std::unordered_map<std::string, std::string>>;
  UPtrMapSS2 m3;

  FP1 fun1;
  FP2 fun2;
}
}  // namespace n1

namespace n2 {
// n1并不能说明using比typedef要好，但对于模板，using要比typedef好，
// 尤其是声明别名有可能是模板化的，typedef做不到。

struct Widget {};

template <typename T>
class MyAlloctor {
 public:
  typedef T value_type;
  typedef T* pointer;
  typedef size_t size_type;
  pointer allocate(size_type _n, const void* hit = 0) {
    return _allocate(_n, (pointer)0);
  }
  void deallocate(pointer p, size_type _n) {}
};

namespace test1 {
// C++98中需要将typedef嵌入在模板化的struct中才能完成：
template <typename T>
struct MyAllocList {
  typedef std::list<T, MyAlloctor<T>> type;
};

// 如果想在一个模板中使用typedef来完成创建节点类型的任务：
template <typename T>
class A {
 private:
  // 此时MyAllocList<T>::type表示依赖T的类型，C++中依赖类型前必须加typename：
  typename MyAllocList<T>::type list;

  // 编译器遇到MyAllocList<T>时，并不知道这是一个类型，
  // 因为有可能存在一个特殊化的MyAllocList，只是编译器还没遇到，
  // 其中的MyAllocList<T>::type表示的并不是一个类型。
};

class Wine {};
template <>
class MyAllocList<Wine> {
 private:
  enum class WineType { White, Red, Rose };
  WineType type;  // 并不是一个类型
};

void func() { MyAllocList<Widget>::type lw; }
}  // namespace test1

namespace test2 {
template <typename T>
using MyAllocList = std::list<T, MyAlloctor<T>>;

template <typename T>
class A {
 private:
  MyAllocList<T> list;  // 使用using后，不用在加typename

  // MyAllocList<T>看上去依赖参数类型T，但编译器在处理A时，
  // 知道MyAllocList<T>是一个类型名称，因为MyAllocList是模板别名，
  // 也必须是一个类型，因此MyAllocList<T>是非依赖类型，不需要typename。
};

void func() { MyAllocList<Widget> lw; }
}  // namespace test2

void func() {
  test1::func();
  test2::func();
}
}  // namespace n2

namespace n3 {
// 如果做过模板元编程TMP，会强烈反对对使用模板类型参数并在此基础上修改为其他类型的必要性：
// 1.去掉const或引用，把const T&变成T；
// 2.添加const或者左值引用，把T变成const T或者T&。

// C++11提供了工具来完成这类转换工作，表现的形式是type traits，
// 它是<type_traits>中的一个模板的分类工具：
// std::remove_const<T>::type，从const T得到T；
// std::remove_reference<T>::type，从T&或T&&得到T；
// std::add_lvalue_reference<T>::type，从T得到T&。
// C++11的类型特征是通过内嵌typedef到一个模板化的struct来实现的，
// 因此当在模板中使用类型参数时，必须添加typename前缀，这是历史遗留问题。

// 对于C++11中的每个类型转换，有一个对应的C++14的模板别名：
// std::remove_const<T>::type          <==>  std::remove_const_t<T>
// std::remove_reference<T>::type      <==>  std::remove_reference_t<T>
// std::add_lvalue_reference<T>::type  <==>  std::add_lvalue_reference_t<T>

// C++11的结构在C++14中依然有效，即使没有C++14，也可以自己写出模板别名：
template <typename T>
using remove_const_t = typename std::remove_const<T>::type;
template <typename T>
using remove_reference_t = typename std::remove_reference<T>::type;
template <typename T>
using add_lvalue_reference_t = typename std::add_lvalue_reference<T>::type;
}  // namespace n3

// 要记住的东西：
// 1.typedef不支持模板化，但是using支持；
// 2.模板别名避免了::type后缀，在模板中，typedef还经常要求使用typename前缀；
// 3.C++14为C++11中的类型特征转换提供了模板别名。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 0]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}