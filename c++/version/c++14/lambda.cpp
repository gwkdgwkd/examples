#include <iostream>
#include <memory>

namespace n1 {
// 泛型lambda：
// 允许在参数列表中使用auto关键字，使lambda能够自动推导参数类型，类似于模板函数的功能。

// 核心优势
// 代码极简‌：单Lambda适配多种类型，避免为不同类型重写相同逻辑。
// 静态类型安全‌：编译时完成类型检查，无运行时开销。
// 混合运算支持‌：自动处理跨类型运算（如int与double）。
// 与STL深度集成‌：常用于泛型算法，如std::sort的自定义比较器。

// 典型应用场景
// 通用计算器‌：单Lambda处理所有数值类型运算。
// 容器操作‌：适配不同元素类型的std::vector或std::map1。
// 回调封装‌：在事件处理中简化多态回调的实现。

// 实现原理：
// 编译器会将泛型Lambda转换为匿名类模板，每个auto参数生成对应的模板参数，
// 最终实例化为具体类型的函数对象。
// 与函数模板相比，泛型Lambda更适用于局部逻辑封装，
// 尤其在需要捕获局部变量时（通过捕获列表[=]或[&]）更具优势。

void func() {
  // 该表达式可处理int、double等任意支持比较运算的类型。
  // 相比C++11必须显式指定参数类型的Lambda，泛型Lambda减少了重复代码量达90%。
  auto glambda = [](auto a, auto b) { return a < b; };
  bool b = glambda(3, 3.14);  // 比较int和double类型
  std::cout << std::boolalpha << b << std::endl;
  // true
}
}  // namespace n1

namespace n2 {
// 初始化捕获：
// 支持在捕获列表中直接初始化新变量，这种捕获方式称为"广义捕获"或"初始化捕获"。

// 实现原理：编译器会将初始化捕获转换为闭包类的成员变量。

// 该特性显著提升了Lambda的资源管理能力和代码简洁性，尤其在需要精确控制捕获行为时优势明显。

void func1() {
  // 通过[新变量名=表达式]的语法，可在捕获时初始化Lambda内部变量。
  int x = 10;
  auto lambda = [value = x + 5] { return value; };
  std::cout << lambda() << std::endl;  // 15
  // 此特性支持值捕获、引用捕获及移动语义，且表达式在Lambda创建时求值。
}

// 典型应用场景：
void func2() {
  // 1.移动语义捕获‌，可捕获仅移动类型：
  auto ptr = std::make_unique<int>(10);
  auto lambda1 = [p = std::move(ptr)] { return *p; };

  // 2.别名捕获‌，为外部变量创建别名引用，避免捕获整个对象：
  struct Widget {
    int data;
  };
  Widget w;
  auto lambda2 = [&d = w.data] { d = 42; };

  // 3.延迟初始化，捕获时执行复杂计算或资源分配：
  auto lambda3 = [buf = new char[1024]] {
    // 使用buf
    delete[] buf;
  };
}

void func() {
  func1();
  func2();
}
}  // namespace n2

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 9]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}