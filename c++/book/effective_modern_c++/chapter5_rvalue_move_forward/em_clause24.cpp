#include <iostream>
#include <vector>

// 条款24：区分通用引用与右值引用

struct Widget {};

namespace n1 {
// 为了声明一个指向某个类型T的右值引用，使用了T&&，由此，一个合理的假设是，
// 当看到一个T&&出现在源码中，看到的就是一个右值引用，但事情并不如此简单。

// 事实上，T&&有两种不同的意思：
// 1.右值引用，这种引用表现得正如所期待的那样，它们只绑定到右值上，
//   并且它们主要的存在原因就是为了识别可以移动操作的对象；
// 2.T&&的另一种意思是，它既可以是右值引用，也可以是左值引用，
//   这种引用看起来像右值引用，即T&&，但是它们可以表现得像是左值引用，即T&，
//   它们的二重性使它们既可以绑定到右值上，也可以绑定到左值上，
//   此外，它们还可以绑定到const或者non-const的对象上，
//   也可以绑定到volatile或者non-volatile的对象上，
//   甚至可以绑定到既const又volatile的对象上，它们可以绑定到几乎任何东西，
//   这种空前灵活的引用值得拥有自己的名字，叫做通用引用，或者转发引用。

// 在两种情况下会出现通用引用：
// 1.最常见的一种是函数模板形参；
// 2.auto声明符。
// 这两种情况的共同之处就是都存在类型推导。

void f1(Widget&& param);  // 右值引用

template <typename T>
void f2(std::vector<T>&& param) {}  // 右值引用

// 在模板f的内部，param的类型需要被推导：
template <typename T>
void f3(T&& param) {}  // 通用引用

// const修饰符的出现，也会使一个引用失去成为通用引用的资格：
template <typename T>
void f4(const T&& param);  // 右值引用

Widget&& var1 = Widget();  // 右值引用

// 变量var2的声明中，var2的类型也需要被推导：
auto&& var2 = var1;  // 通用引用

void func() {
  // 通用引用是引用，所以必须初始化，初始值决定了它是代表了右值引用还是左值引用：
  // 1.如果初始值是一个右值，那么通用引用就会是对应的右值引用；
  // 2.如果初始值是一个左值，那么通用引用就会是一个左值引用。
  // 对那些是函数形参的通用引用来说，初始值在调用函数的时候被提供：
  Widget w;
  f3(w);  // 传递给函数左值，param的类型将会是Widget&，也即左值引用
  f3(std::move(w));  // 传递给函数右值，param的类型会是Widget&&，即右值引用

  // 对一个通用引用而言，类型推导是必要的，但是它还不够，
  // 引用声明的形式必须正确，并且该形式是被限制的，必须恰好为T&&。
  // 当函数f2被调用的时候，如果调用者没有显式地指定它，类型T会被推导。
  // 但是param的类型声明并不是T&&，而是一个std::vector<T>&&，不是通用引用。
  // param因此是右值引用，当向函数f2传递左值时，编译器会报错：
  std::vector<int> v;
  // f2(v);
  // cannot bind rvalue reference of type ‘std::vector<int>&&’ to lvalue of type
  // ‘std::vector<int>’
}
}  // namespace n1

namespace n2 {
// 如果在一个模板里面看见了一个函数形参类型为T&&，也许觉得它是一个通用引用。
// 错，这是由于在模板内部并不保证一定会发生类型推导。

// 标准库中的vector：
template <class T, class Allocator = std::allocator<T>>
class vector {
 public:
  // push_back的形参当然有一个通用引用的正确形式，但并没有发生类型推导：
  // 因为push_back在有一个特定的vector实例之前不可能存在，
  // 而实例化vector时的类型已经决定了push_back的声明。
  void push_back(T&& x) {}

  // 类型参数Args（实际上是一个变参模板）是独立于vector的类型参数T的，
  // 所以Args会在每次emplace_back被调用的时候被推导。
  // 虽然函数emplace_back的类型参数被命名为Args，但是它仍然是一个通用引用，
  // 通用引用的格式必须是T&&，使用的名字T并不是必要的。
  template <class... Args>
  void emplace_back(Args&&... args) {}
};

void func() {
  vector<Widget> v;
  // 将会导致vector模板被实例化为以下代码：
  // class vector<Widget, std::allocator<Widget>> {
  // public:
  //     void push_back(Widget&& x);  // push_back没有类型推导，是右值引用
  // };

  v.emplace_back(1, 2, 3);
}
}  // namespace n2

namespace n3 {
#if __cplusplus == 201402L
// 类型声明为auto&&的变量是通用引用，因为会发生类型推导，并且它们具有正确形式T&&。
// auto类型的通用引用不如函数模板形参中的通用引用常见，但是在C++11中常常突然出现。
// 而它们在C++14中出现得更多，因为C++14的lambda表达式可以声明auto&&类型的形参。
auto f = [](auto&& func, auto&&... params) {
  // func是一个通用引用，可以被绑定到任何可调用对象，无论左值还是右值。
  // args是0个或者多个通用引用，它可以绑定到任意数目、任意类型的对象上。
  // 多亏了auto类型的通用引用，函数f可以对近乎任意函数进行调用：
  std::forward<decltype(func)>(func)(std::forward<delctype(params)>(params)...);
};
#endif
}  // namespace n3

// 要记住的东西：
// 1.如果一个函数模板形参的类型为T&&，并且T需要被推导得知，
//   或者如果一个对象被声明为auto&&，这个形参或者对象就是一个通用引用；
// 2.如果类型声明的形式不是标准的type&&，或者如果类型推导没有发生，
//   那么type&&代表一个右值引用；
// 3.通用引用，如果它被右值初始化，就会对应地成为右值引用，
//   如果它被左值初始化，就会成为左值引用。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
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

  return 0;
}