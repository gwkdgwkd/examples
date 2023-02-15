#include <iostream>

// traits，又被叫做特性萃取技术，
// 说得简单点就是提取被传进的对象对应的返回类型，让同一个接口实现对应的功能。
// 因为STL的算法和容器是分离的，两者通过迭代器链接，
// 算法的实现并不知道自己被传进来什么。
// 萃取器相当于在接口和实现之间加一层封装，来隐藏一些细节并协助调用合适的方法，
// 这需要一些技巧（例如，偏特化）。

namespace n1 {
// 在算法中运用迭代器时，很可能会用到其相应型别（迭代器所指之物的类型）。
// 假设算法中有必要声明一个变量，以迭代器所指对象的类型为类型，该怎么办呢？
// 解决方法是：利用function template的参数推导机制。
template <class I, class T>
void Func_impl(I iter, T t) {
  T tmp;
  std::cout << typeid(T).name() << std::endl;
}
template <class I>
inline void Func(I iter) {
  Func_impl(iter, *iter);  // 传入iter和iter所指的值，class自动推导
}
int func() {
  int i;
  Func(&i);  // 1

  double d;
  Func(&d);  // d
}
}  // namespace n1

namespace n2 {
// 迭代器相应型别不只是迭代器所指对象的类型一种而已。
// 根据经验，最常用的相应型别有五种，
// 然而并非任何情况下任何一种都可以利用上述的template参数推导机制来取得。
// 函数的template参数推导机制推导的只是参数，无法推导函数的返回值类型。
// 万一需要推导函数的传回值，就无能为力了。

// 模板参数不能简单地通过类型返回：
// template<class T>
// (*T) Func1(T t){
// 	T tmp;
// }

// C++11支持了尾行的类型推导：
template <typename T>
auto Func2(T t) -> decltype(*t) {
  return *t;
}

template <class T>
class MyIter {
 public:
  typedef T value_type;  // 声明内嵌类型
  MyIter(T *t) : p(t) {}
  T &operator*() { return *p; }

 private:
  T *p;
};

namespace test1 {
template <class I>
typename I::value_type Func(I ite) {
  return *ite;
}
void func() {
  MyIter<int> i1(new int(8));
  std::cout << Func(i1) << std::endl;  // 8

  MyIter<double> i2(new double(14.5));
  std::cout << Func(i2) << std::endl;  // 14.5

  // 并不是所有迭代器都是class type，原生指针就不行：
  int i = 5;
  // std::cout << Func(&i) << std::endl;
}
}  // namespace test1

namespace test2 {
// 解决原生指针指向的类型无法定义内嵌类型的问题，需要偏特化：
template <class I>
class iterator_traits {
 public:
  typedef typename I::value_type value_type;
};
template <class I>
class iterator_traits<I *> {  // 偏特化是在特化上再加一点限制，但还是特化
 public:
  typedef I value_type;
};
template <class I>
typename iterator_traits<I>::value_type Func(I ite) {
  return *ite;
}
void func() {
  MyIter<int> i1(new int(8));
  std::cout << Func(i1) << std::endl;  // 8

  MyIter<double> i2(new double(14.5));
  std::cout << Func(i2) << std::endl;  // 14.5

  int i = 5;
  std::cout << Func(&i) << std::endl;  // 5
}
}  // namespace test2
void func() {}
}  // namespace n2

namespace n3 {
// traits的作用：
// 1.一方面，在面对不同的输入类时，能找到合适的返回型别；
// 2.另一方面，当型别对应有不同的实现函数的时候，能起到一个提取型别然后分流的作用。

struct A {};
// 继承的好处就是，当函数需A而传入B时，可以往上一直找到适合的对象：
struct B : A {};

template <class AorB>
struct unknown_class {  // 未知类
  typedef AorB return_type;
};

template <class unknown_class>
struct unknown_class_traits {  // 特性萃取器
  typedef typename unknown_class::return_type return_type;
};
template <class T>
struct unknown_class_traits<T *> {  // 特性萃取器，针对原生指针
  typedef T return_type;
};
template <class T>
struct unknown_class_traits<const T *> {  // 特性萃取其，针对指向常数
  typedef const T return_type;
};

// 注释了也不报错，为什么？
template <class unknown_class>
inline typename unknown_class_traits<unknown_class>::return_type return_type(
    unknown_class) {  // 决定使用哪一个类型
  std::cout << "return_type" << std::endl;
  typedef typename unknown_class_traits<unknown_class>::return_type RT;
  return RT();
}

template <class unknown_class>
inline typename unknown_class_traits<unknown_class>::return_type __Func(
    unknown_class, A) {
  std::cout << "use A flag" << std::endl;
  return A();
}
template <class unknown_class>
inline typename unknown_class_traits<unknown_class>::return_type __Func(
    unknown_class, B) {
  std::cout << "use B flag" << std::endl;
  return B();
}
template <class unknown_class, class T>
T __Func(unknown_class, T) {
  std::cout << "use origin ptr" << std::endl;
  return T();
}

// func函数，可以接受自定义的类或者原始的指针作为参数，并自动输出使用了什么tag：
template <class unknown_class>
inline typename unknown_class_traits<unknown_class>::return_type Func(
    unknown_class u) {  // 萃取器取得对应类型
  typedef typename unknown_class_traits<unknown_class>::return_type return_type;
  return __Func(u, return_type());  // 需要调用构造函数当tag
}
void func() {
  unknown_class<A> a;
  A ra = Func(a);  // use A flag
  unknown_class<B> b;
  B rb = Func(b);  // use B flag

  unknown_class<int> i;
  int ri = Func(i);  // use origin ptr

  int value = 1;
  int rpi = Func(&value);  // use origin ptr
}
}  // namespace n3

int main(int argc, char *argv[]) {
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
      n2::test1::func();
      break;
    case 2:
      n2::test2::func();
      break;
    case 3:
      n3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}