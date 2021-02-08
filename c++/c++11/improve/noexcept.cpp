#include <iostream>
#include <vector>

using namespace std;

// 在C语言中，如果程序的运行出现异常、错误，我们想提供方案处理这些异常时，我们面临许多问题，如：
// 1 C语言没有提供统一（标准）的方式来处理错误；
// 2 无法保证错误会被正确的处理；
// 3 错误的传播无法控制，特别是在函数的嵌套调用时；

// C++98中，在函数声明时，我们使用throw指定一个函数可以抛出异常的类型:
class Ex {
 public:
  double getVal();         // 可以抛出任何异常(默认)；
  void display() throw();  // 不可以抛出任何异常；
  void setVal(int i) throw(char*, double);  // 只可以抛出char*和double类型异常。

 private:
  int m_val;
};
// 从功能上来说，C++98中的异常处理机制完全能满足我们的需要，正确的处理异常。
// 然而，编译器为了遵守C++语言标准，在编译时，只检查部分函数的异常规格
// declaration
extern void funAny(void){};                 // May throw ANY exception.
void check(void) throw(std::out_of_range);  // May throw only std::out_of_range.
// implementation
void check(void) throw(std::out_of_range) { funAny(); }
// 程序在运行时，如果funAny()抛出一个异常，但是它的类型不是std::out_of_range，
// 异常处理机制将调用std::unexpected()（该函数自己也可能抛出异常），这个函数默认情况下会调用std::teminate().

// 编译器在编译时能过做的检测非常有限，因此在C++11中异常声明被简化为以下两种情况：
// 1 函数可以抛出任何异常(和之前的默认情况相同)；
// 2 函数不可以抛出任何异常。

// 在C++11中，声明一个函数不可以抛出任何异常使用关键字noexcept:
void mightThrow();             // could throw any exceptions
void doesNotThrow() noexcept;  // does not throw any exception.
// 两个函数声明的异常规格在语义上是相同的，都表示函数不抛出任何异常:
void old_stytle() throw();
void new_style() noexcept;
// 它们的区别在于程序运行时的行为和编译器优化的结果:
// 使用throw()， 如果函数抛出异常，异常处理机制会进行栈回退，寻找(一个或多个）catch语句。
// 此时，检测catch可以捕捉的类型，如果没有匹配的类型，std::unexpected()会被调用。但是std::unexpected()本身也可能抛出异常。
// 如果std::unexpected()抛出的异常对于当前的异常规格是有效的，异常传递和栈回退会像以前那样继续进行。这意味着，如果使用throw， 编译器几乎没有机会做优化。
// 事实上，编译器甚至会让代码变得更臃肿、庞大：
// 1 栈必须被保存在回退表中；
// 2 所有对象的析构函数必须被正确的调用（按照对象构建相反的顺序析构对象）；
// 3 编译器可能引入新的传播栅栏（propagation barriers）、引入新的异常表入口，使得异常处理的代码变得更庞大；
// 4 内联函数的异常规格（exception specification）可能无效的。
// 当使用noexcept时，std::teminate()函数会被立即调用，而不是调用std::unexpected();
// 因此，在异常处理的过程中，编译器不会回退栈，这为编译器的优化提供了更大的空间。
// 简而言之，如果你知道你的函数绝对不会抛出任何异常，应该使用noexcept, 而不是throw().

// constexpr initializer_list() noexcept : _M_array(0), _M_len(0) {}
// noexcept该关键字告诉编译器，函数中不会发生异常,这有利于编译器对程序做更多的优化。如果在运行时，noexecpt函数向外抛出了异常（如果函数内部捕捉了
// 异常并完成处理，这种情况不算抛出异常），程序会直接终止，调用std::terminate()函数，该函数内部会调用std::abort()终止程序。

// 单独使用noexcept，表示其所限定的swap函数绝对不发生异常。然而，使用方式可以更加灵活，表明在一定条件下不发生异常:
// void swap(Type& x, Type& y) noexcept(noexcept(x.swap(y))) { x.swap(y); }
// 如果操作x.swap(y)不发生异常，那么函数swap(Type& x, Type& y)一定不发生异常。
// std::pair中的移动分配函数（move assignment），如果类型T1和T2的移动分配（move assign）过程中不发生异常，那么该移动构造函数就不会发生异常:
// pair& operator=(pair&& __p) noexcept(
//     __and_<is_nothrow_move_assignable<_T1>,
//            is_nothrow_move_assignable<_T2>>::value) {
//   first = std::forward<first_type>(__p.first);
//   second = std::forward<second_type>(__p.second);
//   return *this;
// }

// 什么时候该使用noexcept(在不是以下情况或者没把握的情况下，不要轻易使用noexception):
// 使用noexcept表明函数或操作不会发生异常，会给编译器更大的优化空间。然而，并不是加上noexcept就能提高效率，以下情形鼓励使用noexcept：
// 移动构造函数（move constructor）
// 移动分配函数（move assignment）
// 析构函数（destructor）,在新版本的编译器中，析构函数是默认加上关键字noexcept的。
// 叶子函数（Leaf Function）。叶子函数是指在函数内部不分配栈空间，也不调用其它函数，也不存储非易失性寄存器，也不处理异常。

// 下面代码可以检测编译器是否给析构函数加上关键字noexcept:
struct X {
  ~X(){};
};

void may_throw();
void no_throw() noexcept;
auto lmay_throw = [] {};
auto lno_throw = []() noexcept {};
class T {
 public:
  ~T() {}  // 析构函数防止移动构造函数
           // 复制构造函数是noexcept
};
class U {
 public:
  ~U() {}  // 析构函数防止移动构造函数
           // 复制构造函数是noexcept(false)
  std::vector<int> v;
};
class V {
 public:
  std::vector<int> v;
};

int main() {
  X x;
  // This will not fire even in GCC 4.7.2 if the destructor is explicitly marked as noexcept(true)
  static_assert(noexcept(x.~X()), "Ouch!");

  T t;
  U u;
  V v;
  std::cout
      << std::boolalpha << "Is may_throw() noexcept? " << noexcept(may_throw())
      << '\n'
      << "Is no_throw() noexcept? " << noexcept(no_throw()) << '\n'
      << "Is lmay_throw() noexcept? " << noexcept(lmay_throw()) << '\n'
      << "Is lno_throw() noexcept? " << noexcept(lno_throw()) << '\n'
      << "Is ~T() noexcept? " << noexcept(std::declval<T>().~T()) << '\n'
      // note: the following tests also require that ~T() is noexcept because
      // the expression within noexcept constructs and destroys a temporary
      << "Is T(rvalue T) noexcept? " << noexcept(T(std::declval<T>())) << '\n'
      << "Is T(lvalue T) noexcept? " << noexcept(T(t)) << '\n'
      << "Is U(rvalue U) noexcept? " << noexcept(U(std::declval<U>())) << '\n'
      << "Is U(lvalue U) noexcept? " << noexcept(U(u)) << '\n'
      << "Is V(rvalue V) noexcept? " << noexcept(V(std::declval<V>())) << '\n'
      << "Is V(lvalue V) noexcept? " << noexcept(V(v)) << '\n';

  // Is may_throw() noexcept? false
  // Is no_throw() noexcept? true
  // Is lmay_throw() noexcept? false
  // Is lno_throw() noexcept? true
  // Is ~T() noexcept? true
  // Is T(rvalue T) noexcept? true
  // Is T(lvalue T) noexcept? true
  // Is U(rvalue U) noexcept? false
  // Is U(lvalue U) noexcept? false
  // Is V(rvalue V) noexcept? true
  // Is V(lvalue V) noexcept? false

  return 0;
}