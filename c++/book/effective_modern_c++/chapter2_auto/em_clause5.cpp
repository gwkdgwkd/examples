#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// 第二章：auto关键字

// 条款5：优先使用auto而非显式类型声明

namespace n1 {
void func() {
  int x1;  // 忘记了初始化，没有报错
  // auto x2;      // 必须初始化，如果忘记了报错
  auto x3 = 0;  // 没有忘记初始化，正确
}
}  // namespace n1

namespace n2 {
// 想要通过迭代器解引用来初始化局部变量

namespace test1 {
template <typename It>
void dwim(It b, It e) {
  while (b != e) {
    // std::iterator_traits<It>::value_type表示迭代器指向的值的类型：
    typename std::iterator_traits<It>::value_type currValue = *b;
  }
}
}  // namespace test1

namespace test2 {
template <typename It>
void dwim(It b, It e) {
  while (b != e) {
    auto currValue = *b;
  }
}
}  // namespace test2
}  // namespace n2

namespace n3 {
// 由于auto使用模板类型推导，它可以表示那些仅仅被编译器知晓的类型

struct Widget {
  bool operator<(const Widget& rhs) { return true; }
};

void func() {
  auto dereUPLess1 = [](const std::unique_ptr<Widget>& p1,
                        const std::unique_ptr<Widget>& p2) {
    return *p1 < *p2;
  };

  // 在C++14中，模板被进一步丢弃，因为使用lambda表达式的参数可以包含auto：
  auto dereUPLess2 = [](const auto& p1, const auto& p2) { return *p1 < *p2; };

  // 如果不使用auto：
  std::function<bool(const std::unique_ptr<Widget>&,
                     const std::unique_ptr<Widget>&)>
      dereUPLess3 = [](const std::unique_ptr<Widget>& p1,
                       const std::unique_ptr<Widget>& p2) { return *p1 < *p2; };

  // 使用std::function和auto并不一样：
  // 1.使用auto声明的变量和封装体有同样的类型，也仅使用和封装体同样大小的内存，
  //   std::function声明的变量是std::function模板的一个实例，
  //   并且对任何类型只有一个固定的大小，这个内存的大小可能不满足封装体的需求，
  //   出现这种情况时，std::function将会开辟堆控件来存储这个封装体，
  //   导致的结果就是std::function对象一般会比auto声明的对象使用更多的内存；
  // 2.通过std::function对象来调用一个封装体比通过auto对象要慢。
  // 总之，std::function方法通常体积比auto大，并且慢，还有可能导致内存不组的异常。
}
}  // namespace n3

namespace n4 {
// auto的优点除了可以避免未初始化变量，变量声明引起的歧义，
// 直接持有封装体的能力，还有一个就是可以避免类型截断的问题。

void func() {
  std::vector<int> v;
  unsigned sz = v.size();

  // v.size()返回的是std::vector<int>::size_type，但很多人都不知道。
  // std::vector<int>::size_type是无符号的，导致好多人以为unsigned足够了。
  // 32位系统，std::vector<int>::size_type和unsigned有同样的大小，
  // 但在64位系统，std::vector<int>::size_type是64位的。
  // 这意味这上面的代码在32位系统工作良好，但在64位系统有时可能不正确。

  // 使用auto可以保证不被上面的问题所困扰：
  auto size = v.size();
}
}  // namespace n4

namespace n5 {
void func() {
  std::unordered_map<std::string, int> m;
  for (const std::pair<std::string, int>& p : m) {
  }

  // 上面代码看上去合理，但有一个问题：
  // std::unorder_map的key是const，不是std::pair<std::string,int>，
  // 而是std::pair<const std::string,int>，但这不是p的声明类型，
  // 后果就是编译器竭尽全力把哈希表中的std::pair<const std::string,int>转换，
  // 转成p声明的类型std::pair<std::string,int>。
  // 这个过程将通过复制m的一个元素到临时对象，然后将临时对象和p绑定完成，
  // 在每个循环结束时这个临时对象将被销毁，代码行为与预期不符。

  // 通过auto解决上面的问题：
  for (const auto& p : m) {  // 不仅高效而且简洁
  }
}
}  // namespace n5

// 有好几个使用auto而不是显式类型声明的原因，然后auto并不完美，
// auto都是由初始化的表达式推导出来的，一个初始化类型不是期望的类型。

// auto是个可选项，不是必选项，如果不用auto更清晰更容易维护，那就不用。

// 显式写出类型可能会引入一些难以察觉的错误，导致正确性或效率问题，或兼而有之。
// 此外，如果初始化auto的表达式改变了，auto会自动改变，意味着代码重构更简单。
// 例如，一个函数被声明为返回int，但后面要改成long，如果使用了auto，
// 调用代码自动更新，如果不使用auto，那么要修改每个存储显示声明为int的变量。

// 要记住的东西：
// 1.auto变量一定要初始化，并且对由于类型不匹配引起的兼容和效率问题有免疫力，
//   可以简化代码结构，一般会比显式的声明类型敲击更少的键盘；
// 2.auto类型的变量也受限于条款2和条款6中描述的陷阱。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n3::func();
      break;
    case 2:
      n4::func();
      break;
    case 3:
      n5::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}