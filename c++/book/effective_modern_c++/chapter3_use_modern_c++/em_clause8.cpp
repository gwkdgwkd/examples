#include <iostream>
#include <memory>
#include <mutex>

// 条款8：优先使用nullptr而不是0或者NULL

// 0字面上是int类型，而不是指针，这是显而易见的。
// 编译器看到0，但发现上下文仅有指针用到了它，然后勉强将0解释成空指针。
// 这仅仅是个应变之策，C++最初的原则：0是int而非指针。
// NULL也有同样的问题，它和0都不属于指针类型。

namespace n1 {
// 在C++98中，重载指针和整数类型的函数会令人吃惊。

void f(int) { std::cout << "f(int)" << std::endl; }
void f(bool) { std::cout << "f(bool)" << std::endl; }
void f(void*) { std::cout << "f(void*)" << std::endl; }
void func() {
  f(0);  // f(int)

  // f(NULL); // 编译错误
  // 如果NULL是0L，可以转为int，再转为bool，也可以转为void*，冲突。
  // 这就是C++98不允许重载指针和整数类型的原因，这个原则对C++11依然有效。

  // nullptr的优势是它不再是一个整数类型，但也不是一个指针类型，
  // 不过可以把它想象成一个可以指向任意类型的指针。
  // nullptr是std::nullptr_t类型的一个对象，可以隐式转换为所有原始指针类型。
  f(nullptr);  // f(void*)
}
}  // namespace n1

namespace n2 {
// 使用nullptr而不是0或者NULL，可以避免重载解析上的令人吃惊行为，
// 但是它的优势不仅限于此，还可以提高代码的清晰度，尤其是牵扯到auto类型变量时：
// auto result = findRecord();
// if(result == 0) {}
// 如果不知findRecord返回的是什么，那么就不知道result是指针还是整数。

// if(result == nullptr) {}
// 使用nullptr就没有歧义了。
}  // namespace n2

namespace n3 {
// 当考虑到模板时，nullptr的光芒显得更加耀眼了。

struct Widget {};
int f1(std::shared_ptr<Widget> spw) {}
int f2(std::unique_ptr<Widget> upw) {}
int f3(Widget* pw) {}
using MuxGuard = std::lock_guard<std::mutex>;
std::mutex f1m, f2m, f3m;

namespace test1 {
void func() {
  {
    MuxGuard g(f1m);
    auto result = f1(0);
  }
  {
    MuxGuard g(f2m);
    auto result = f2(NULL);
  }
  {
    MuxGuard g(f3m);
    auto result = f3(nullptr);
  }

  // 代码可以工作，但有重复。
}
}  // namespace test1

namespace test2 {
// 使用模板解决代码重复问题：
#if __cplusplus == 201402L
template <typename FuncType, typename MuxType, typename PtrType>
decltype(auto) lockAndCall(FuncType func, MuxType& mutex, PtrType ptr) {
  MuxGuard g(mutex);
  return func(ptr);
}
#elif __cplusplus == 201103L
template <typename FuncType, typename MuxType, typename PtrType>
auto lockAndCall(FuncType func, MuxType& mutex, PtrType ptr)
    -> decltype(func(ptr)) {
  MuxGuard g(mutex);
  return func(ptr);
}
#endif

void func() {
  // auto result1 = lockAndCall(f1, f1m, 0);        // 错误
  // auto result2 = lockAndCall(f2, f2m, NULL);     // 错误
  // 当把0作为参数传给lockAndCall，模板通过类型推导得知是int类型，
  // 意味着调用func传入的是int类型，而f1期望接收std::shared_ptr<Widget>类型，
  // 尝试将int作为std::shared_ptr<Widget>传个f1会导致类型冲突错误。
  // 对NULL的情况是一样的，NULL传给lockAndCall时，被模板当做int类型。

  auto result1 = lockAndCall(f1, f1m, nullptr);  // 正确
  auto result2 = lockAndCall(f2, f2m, nullptr);  // 正确
  auto result3 = lockAndCall(f3, f3m, nullptr);  // 正确
  // 使用nullptr是没问题的，当nullptr传给lockAndCall，
  // ptr被推导 成std::nullptr_t，ptr再传给f3时，
  // std::nullptr_t可以隐式转换为任意类型的指针，也包括Widget*。
}

// 对于0和NULL，模板类型推导出了错误的类型，是它们的真正类型，
// 这是需要用到空指针时使用nullptr而非0或NULL最引人注目的原因。
}  // namespace test2
}  // namespace n3

// 要记住的东西：
// 1.相较于0和NULL，优先使用nullptr；
// 2.避免整数类型和指针类型直接的重载。

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