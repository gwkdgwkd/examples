#include <stdarg.h>
#include <iostream>
#include <typeinfo>

// 可变模版参数（variadic templates），是C++11新增的最强大的特性之一，
// 它对参数进行了高度泛化，它能表示0到任意个数、任意类型的参数。
// 相比C++98/03，类模版和函数模版中只能含固定数量的模版参数，
// 可变模版参数无疑是一个巨大的改进。
// 然而由于可变模版参数比较抽象，使用起来需要一定的技巧，
// 所以它也是C++11中最难理解和掌握的特性之一。
// 虽然掌握可变模版参数有一定难度，但是它却是C++11中最有意思的一个特性。

// variadic template是一个很自然的需求，它完善了C++的模板设计手段。
// 原来的模板参数可以使类和函数的参数类型任意化，如果再加上参数个数的任意化，
// 那么参数的设计手段就基本上齐备了，可以让设计的函数或类有更大的复用性。
// 因为有很多处理都是与处理对象的个数关系不大的：
// 1.比如说打屏（printf）；
// 2.比如说比较大小（max，min）；
// 3.比如函数绑定子，bind，function要对应的函数就可能任意参数个数和类型。

// 如果不能对应任意个参数，那么就总会有人无法重用，而不得不重复地处理，
// 而共通库的实现者为了尽可能地让自己写的类（函数）能复用在更多的场景，
// 也不得不重复地写很多的代码或是用诡异的技巧，比如用宏对应有限个任意参数。

// C++03只有固定模板参数；
// C++11加入新的表示法，允许任意个数、任意类别的模板参数，
// 不必在定义时将参数的个数固定，其标准表示：
// template<typename... Values> class tuple;
// 实参的个数也可以是0，
// 所以tuple<> someInstanceName这样的定义也是可以的。
// 若不希望产生实参个数为0的变长参数模板，则可以采用以下的定义：
// template <typename First, typename... Rest> class tuple;

// Values被称为模版参数包（template parameter pack），
// 可以理解为多个模版参数类型的集合，只要能把这个集合拆开，
// 拆开为单个单个的元素，那么就可以正常使用了。

// 可变模板参数是有它的局限性的，没有办法直接获取包中每一个元素的类型。
// 可变长模板参数的偏特化和全特化都是不支持的。
// 可变模版参数和普通的模版参数语义是一致的，可以应用于函数和类。
// 函数模版不支持偏特化，所以可变参数函数模版和类模版展开参数包的方法不同。

namespace n1 {
namespace test1 {
// 查询参数包的参数个数：
template <typename... Args>
unsigned int length(Args... args) {
  return sizeof...(args);
}

void func() {
  std::cout << length(2, 2.5, "nihao") << std::endl;  // 3
  std::cout << length(6.7f, 'c') << std::endl;        // 2
}
}  // namespace test1

// 展开可变模版参数函数的方法一般有两种：
namespace test2 {
// 1.通过递归函数展开参数包，需要提供参数包展开的函数和递归终止函数；
void print() {  // 递归终止函数
  // 当没有参数时，则调用非模板函数print终止递归过程。
  std::cout << "empty" << std::endl;
}
template <class T, class... Args>
void print(T head, Args... rest) {  // 展开函数
  // 参数包Args...在展开的过程中递归调用自己，
  // 每调用一次参数包中的参数就会少一个，直到所有的参数都展开为止。
  std::cout << "param " << head << " -> ";
  print(rest...);
}
void func() {
  print(1, 2, 3, 4);
  // param 1 -> param 2 -> param 3 -> param 4 -> empty
  print();
  // empty
  print(1, "hello", "C++");
  // param 1 -> param hello -> param C++ -> empty
}

// 递归函数展开参数包是一种标准做法，也比较好理解，
// 但也有一个缺点，就是必须要一个重载的递归终止函数，
// 即必须要有一个同名的终止函数来终止递归，这样可能会感觉稍有不便。
}  // namespace test2

namespace test3 {
// 2.通过逗号表达式展开参数包：
//   其实还有一种方法可以不通过递归方式来展开参数包，
//   这种方式需要借助逗号表达式和初始化列表。
template <class T>
void printarg(T t) {
  // printarg不是递归终止函数，只是处理参数包中每一个参数的函数。
  std::cout << "param " << t << " -> ";
}
template <class... Args>
void print(Args... args) {
  // 不需要通过递归终止函数，是直接在函数体中展开的。
  // 这种就地展开参数包的方式实现的关键是逗号表达式，
  // 逗号表达式会按顺序执行逗号前面的表达式。
  int arr[] = {(printarg(args), 0)...};
  // (printarg(args),0)，先执行printarg(args)，再得到逗号表达式的结果0。
  // 还用到了C++11的一个特性，初始化列表，通过初始化列表来初始化一个变长数组：
  // {(printarg(args),0)...}将会展开成，
  // ((printarg(arg1),0),(printarg(arg2),0),etc...)，
  // 最终会创建一个元素值都为0的数组int arr[sizeof...(Args)]。
  // 也就是说在构造int数组的过程中就将参数包展开了，
  // 这个数组的目的纯粹是为了在数组构造的过程展开参数包。
  std::cout << std::endl;
}

void func() {
  print(1, 2, 3, 4);
  // param 1 -> param 2 -> param 3 -> param 4 ->
  print();
  //
  print(1, "hello", "C++");
  // param 1 -> param hello -> param C++ ->
}
}  // namespace test3

namespace test4 {
// 再进一步改进一下，将函数作为参数，就可以支持lambda表达式了，
// 从而可以少写一个递归终止函数了：
template <class F, class... Args>
void print(const F& f, Args&&... args) {
  // 用到了完美转发：
  std::initializer_list<int>{(f(std::forward<Args>(args)), 0)...};
}

void func() {
  print([](int i) { std::cout << "param " << i << " -> "; }, 1, 2, 3, 4);
  std::cout << std::endl;
  // param 1 -> param 2 -> param 3 -> param 4 ->
}
}  // namespace test4
}  // namespace n1

namespace n2 {
// 可变参数模板类是一个带可变模板参数的模板类，
// 比如C++11中的元组std::tuple就是一个可变模板类。
// 可变参数模板类的参数包展开的方式和可变参数模板函数的展开方式不同，
// 可变参数模板类的参数包展开需要通过模板特化和继承方式去展开，
// 展开方式比可变参数模板函数要复杂。

// 1.模版偏特化和递归方式来展开参数包，
//   一般需要定义两到三个类，包括类声明和偏特化的模板类。
namespace test1 {
// 前向声明，声明这个sum类是一个可变参数模板类：
template <typename... Args>
struct Sum;
// 定义了部分展开的可变模参数模板类，告诉编译器如何递归展开参数包：
template <typename First, typename... Rest>
struct Sum<First, Rest...> {
  enum { value = Sum<First>::value + Sum<Rest...>::value };
};
// 递归终止，特化的递归终止类：
template <typename Last>
struct Sum<Last> {
  enum { value = sizeof(Last) };
};
// 这个前向声明要求模板参数至少有一个，因为可变参数模板中的模板参数可以有0个，
// 有时候0个模板参数没有意义，就可以通过上面的声明方式来限定模板参数不能为0个。

void func() {
  std::cout << Sum<int, double, short>::value << std::endl;        // 14
  std::cout << Sum<int, double, short, char>::value << std::endl;  // 15
}
}  // namespace test1

namespace test2 {
// 上面的这种三段式的定义也可以改为两段式的，可以将前向声明去掉：
template <typename First, typename... Rest>
struct Sum {
  enum { value = Sum<First>::value + Sum<Rest...>::value };
};
template <typename Last>
struct Sum<Last> {
  enum { value = sizeof(Last) };
};
// 上面的方式只要一个基本的模板类定义和一个特化的终止函数就行了，
// 而且限定了模板参数至少有一个。

void func() {
  std::cout << Sum<int, double, short>::value << std::endl;        // 14
  std::cout << Sum<int, double, short, char>::value << std::endl;  // 15
}
}  // namespace test2

namespace test3 {
// 递归终止模板类可以有多种写法，比如上例的递归终止模板类还可以这样写：
template <typename... Args>
struct Sum;
template <typename First, typename Last>
struct Sum<First, Last> {
  enum { value = sizeof(First) + sizeof(Last) };
};
void func() {
  // std::cout << Sum<int, double, short>::value << std::endl;        // 14
  // std::cout << Sum<int, double, short, char>::value << std::endl;  // 15
}

// 在展开到最后两个参数时终止，还可以再展开到0个参数时终止：
// template<>struct sum<> { enum{ value = 0 }; };
// 还可以使用std::integral_constant来消除枚举定义value。
// 利用std::integral_constant可以获得编译期常量的特性，
// 可以将前面的sum改为：
// 前向声明：
// template <typename First, typename... Args>
// struct Sum;
// 基本定义：
// template <typename First, typename... Rest>
// struct Sum<First, Rest...>
//     : std::integral_constant<int,
//       Sum<First>::value + Sum<Rest...>::value> {};
// 递归终止
// template <typename Last>
// struct Sum<Last> : std::integral_constant<int, sizeof(Last)> {};
}  // namespace test3

// 2.继承方式展开参数包
// 整型序列的定义：
template <int...>
struct IndexSeq {};
namespace test4 {
// 继承方式，开始展开参数包：
template <int N, int... Indexes>
struct MakeIndexes : MakeIndexes<N - 1, N - 1, Indexes...> {};
// 模板特化，终止展开参数包的条件：
template <int... Indexes>
struct MakeIndexes<0, Indexes...> {
  typedef IndexSeq<Indexes...> type;
};
// 其中MakeIndexes的作用是为了生成一个可变参数模板类的整数序列，
// 最终输出的类型是：struct IndexSeq<0,1,2>。
// MakeIndexes继承于自身的特化模板类，这个特化模板类也在展开参数包，
// 这个展开过程是通过继承发起的，直到遇到特化的终止条件展开过程才结束。
void func() {
  using T = MakeIndexes<3>::type;
  std::cout << typeid(T).name() << std::endl;
  // N2n28IndexSeqIJLi0ELi1ELi2EEEE

  MakeIndexes<3, 4, 5, 6> a;
}
}  // namespace test4

namespace test5 {
// 如果不希望通过继承方式去生成整形序列，则可以通过下面的方式生成：
template <int N, int... Indexes>
struct MakeIndexes {
  using type = typename MakeIndexes<N - 1, N - 1, Indexes...>::type;
};
template <int... Indexes>
struct MakeIndexes<0, Indexes...> {
  typedef IndexSeq<Indexes...> type;
};

void func() { MakeIndexes<3, 4, 5, 6> a; }
}  // namespace test5
}  // namespace n2

namespace n3 {
// 可变参数模版消除重复代码
struct A {
  A(int) {}
};
struct B {
  B(int, double) {}
};

namespace test1 {
// C++11之前如果要写一个泛化的工厂函数，这个工厂函数能接受任意类型的入参，
// 并且参数个数要能满足大部分的应用需求的话，不得不定义很多重复的模版定义：
template <typename T>
T* Instance() {
  return new T();
}
template <typename T, typename T0>
T* Instance(T0 arg0) {
  return new T(arg0);
}
template <typename T, typename T0, typename T1>
T* Instance(T0 arg0, T1 arg1) {
  return new T(arg0, arg1);
}
template <typename T, typename T0, typename T1, typename T2>
T* Instance(T0 arg0, T1 arg1, T2 arg2) {
  return new T(arg0, arg1, arg2);
}
template <typename T, typename T0, typename T1, typename T2, typename T3>
T* Instance(T0 arg0, T1 arg1, T2 arg2, T3 arg3) {
  return new T(arg0, arg1, arg2, arg3);
}
template <typename T, typename T0, typename T1, typename T2, typename T3,
          typename T4>
T* Instance(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4) {
  return new T(arg0, arg1, arg2, arg3, arg4);
}

void func() {
  A* pa = Instance<A>(1);
  B* pb = Instance<B>(1, 2);
}
}  // namespace test1

namespace test2 {
// 上面的泛型工厂函数存在大量的重复的模板定义，并且限定了模板参数。
// 用可变模板参数可以消除重复，同时去掉参数个数的限制，代码很简洁：
template <typename T, typename... Args>
T* Instance(Args&&... args) {
  return new T(std::forward<Args>(args)...);
}

void func() {
  A* pa = Instance<A>(1);
  B* pb = Instance<B>(1, 2);
}
}  // namespace test2

void func() {
  test1::func();
  test2::func();
}
}  // namespace n3

namespace n4 {
// 可变参数模版实现泛化的delegate，C++中没有类似C#的委托，
// 可以借助可变模版参数来实现一个：
template <class T, class R, typename... Args>
class MyDelegate {
 public:
  MyDelegate(T* t, R (T::*f)(Args...)) : m_t(t), m_f(f) {}

  R operator()(Args&&... args) {
    return (m_t->*m_f)(std::forward<Args>(args)...);
  }

 private:
  T* m_t;
  // 实现的关键是定义了一个能接受任意类型和个数参数的万能函数：
  R (T::*m_f)(Args...);
};
template <class T, class R, typename... Args>
MyDelegate<T, R, Args...> CreateDelegate(T* t, R (T::*f)(Args...)) {
  return MyDelegate<T, R, Args...>(t, f);
}
struct A {
  void Fun1(int i) { std::cout << i << std::endl; }
  void Fun2(int i, double j) { std::cout << i + j << std::endl; }
};

// 使用可变模版参数的这些技巧让人耳目一新，使用可变模版参数的关键是如何展开参数包，
// 展开参数包的过程是很精妙的，体现了泛化之美、递归之美，正是因为它具有神奇的魔力，
// 可以更泛化的去处理问题，比如消除重复的模版定义，定义能接受任意参数的万能函数等。
// 其实，可变模版参数的作用远不止列举的那些作用，它还可以和其它C++11特性结合起来，
// 比如type_traits、std::tuple等特性，发挥更加强大的威力，特别是在模板元编程中。

void func() {
  A a;
  auto d1 = CreateDelegate(&a, &A::Fun1);  // 创建委托
  d1(1);                                   // 1
  auto d2 = CreateDelegate(&a, &A::Fun2);  // 创建委托
  d2(1, 2.5);                              // 3.5
}
}  // namespace n4

namespace n5 {
// c++可变长参数和__VA_ARGS__

// C++11已经支持了C99的变长宏，变长宏与printf的默契配合，
// 使得程序员能够非常容易地派生出printf的变种以支持一些记录。
// 而printf则使用C语言的函数变长参数特性，通过使用变长函数，
// printf的实现能够接受任何长度的参数列表。
// 不过无论是宏，还是变长参数，都无法获得传递参数的类型。

// _VA_ARGS：
#define LOG(...) printf(__VA_ARGS__)  // 三个点

// 可变长参数列表：函数的参数是以压栈的方式进行的，从右到左进行压栈。
double SumofFlot(int count, ...) {
  va_list ap;
  double sum = 0;
  va_start(ap, count);  // 获得变长列表的句柄ap
  for (int i = 0; i < count; ++i) {
    sum += va_arg(ap, double);  // 每次获得一个参数
  }

  va_end(ap);
  return sum;
}
// 为什么要放一个int count呢?
// 函数参数是以压栈的方式来存储的，这是一块连续的内存，并且count是首地址。
// 那么肯定是需要知道知道栈的首地址的，有了int count就意味着有了首地址。
// 并且借助下面三点，就可以完成的遍历参数列表了：
// 1.连续内存的首地址；
// 2.下一个元素的类型；
// 3.列表的长度。

// 处理可变参数的标准宏：
// 1.va_list pvar;
//   获取可变长参数列表中的元素。
// 2.va_start(pvar,parM);
//   让pvar指向参数parM对应的地址，parM应该是最靠近可变长参数列表的元素。
// 3.va_arg(pvar, type);
//   提取pvar的值，type指定参数的类型，提取pvar后，指向下一个元素的地址。
// 4.va_end(pvar);
//   结束对参数列表的访问。

// 使用可变长参数是很有局限性的，其中之一就是：va_arg(var, int)，
// 需要指定参数的类型才能进行正确的取到下一个参数的内容。
// 而且无法知道可变参数的长度，所以必须事先约定好参数的类型和长度，
// 并且需要传递进函数里面去告诉函数，这样在灵活性上就大打折扣了。

// 变长函数这种实现方式，对于C++这种强调类型的语言来说相当于开了不规范的后门。

void func() {
  LOG("my name is:%s\n", "zhangkai");  // my name is:zhangkai

  std::cout << SumofFlot(2, 2.4, 4.5) << std::endl;            // 6.9
  std::cout << SumofFlot(4, 2.4, 4.5, 0.1, 2.0) << std::endl;  // 9
}
}  // namespace n5

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 11]" << std::endl;
    return 0;
  }
  int type = atoi(argv[1]);
  switch (type) {
    case 0:
      n1::test1::func();
      break;
    case 1:
      n1::test2::func();
      break;
    case 2:
      n1::test3::func();
      break;
    case 3:
      n1::test4::func();
      break;
    case 4:
      n2::test1::func();
      break;
    case 5:
      n2::test2::func();
      break;
    case 6:
      n2::test3::func();
      break;
    case 7:
      n2::test4::func();
      break;
    case 8:
      n2::test5::func();
      break;
    case 9:
      n3::func();
      break;
    case 10:
      n4::func();
      break;
    case 11:
      n5::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}