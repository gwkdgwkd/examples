#include <iostream>
#include <vector>

namespace n1 {
// 在C++98/03中的对象初始化方法有很多种,
// 这些不同的初始化方法，都有各自的适用范围和作用。
// 最关键的是，这些种类繁多的初始化方法，没有一种可以通用所有情况。

// 1.初始化列表:
int i_arr[3] = {1, 2, 3};  // 普通数组
struct A {
  int x;
  struct B {
    int i;
    int j;
  } b;
} a = {1, {2, 3}};  // POD类型,简单来说，是可以直接使用memcpy复制的对象。
// POD全称PlainOldData。
// 通俗的讲，一个类或结构体通过二进制拷贝后还能保持其数据不变，那么它就是一个POD类型。
// 对于普通数组和POD类型，C++98/03可以使用初始化列表（initializerList）进行初始化，
// 但是这种初始化方式的适用性非常狭窄，只有这两种数据类型可以使用初始化列表。

// 2.拷贝初始化（copy-initialization）:
int i = 0;
class C {
 public:
  C(int) {}
} c1 = 123;  // 需要拷贝构造函数

// 3.直接初始化（direct-initialization）:
int j(0);
C c2(123);

}  // namespace n1

namespace n2 {
// 为了统一初始化方式，并且让初始化行为具有确定的效果，
// C++11中提出了列表初始化（List-initialization）的概念。

// 在C++11中，可以直接在变量名后面跟上初始化列表，来进行对象的初始化。
// 在初始化时，{}前面的等于号是否书写对初始化行为没有影响。
// 变量名后面跟上初始化列表方法同样适用于普通数组和POD类型的初始化:
int arr1[3]{1, 2, 3};     // 普通数组
int arr2[3] = {1, 2, 3};  // 普通数组
struct A {
  int x;
  struct B {
    int i;
    int j;
  } b;
} a1{1, {2, 3}};  // POD类型
A a2 = {1, {2, 3}};

// 列表初始化还可以直接使用在函数的返回值上，
// return语句就如同返回了一个Foo(123,321.0)。
struct Foo {
  Foo(int, double) {}
};
Foo func(void) { return {123, 321.0}; }
}  // namespace n2

namespace n3 {
// 在C++11中使用初始化列表是非常便利的。
// 它不仅统一了各种对象的初始化方式，而且还使代码的书写更加简单清晰。
class Foo {
 public:
  Foo(int) {}

 private:
  Foo(const Foo &);
};

void func1() {
  // 在C++11中，初始化列表的适用性被大大增加了，它现在可以用于任何类型对象的初始化。
  Foo a1(123);
  // Foo a2 = 123; // error: 'Foo::Foo(const Foo &)' is private

  // a3、a4使用了新的初始化方式来初始化对象，效果如同a1的直接初始化。
  Foo a3 = {123};  // 虽然使用了等号，但仍是列表初始化，不受私有拷贝构造影响到。
  Foo a4{123};  // 直接在变量名后面跟上初始化列表，C++98/03所不具备的。

  // a5、a6是基本数据类型的列表初始化方式。
  int a5 = {3};
  int a6{3};
}

void func2() {
  // new操作符等可以用圆括号进行初始化的地方，也可以使用初始化列表:
  int *a = new int{123};  // 通过初始化列表方式在内存初始化时指定了值为123
  double b = double{12.12};  // 对匿名对象使用列表初始化后，再进行拷贝初始化
  int *arr = new int[3]{1, 2, 3};  // 堆上动态分配的数组也可以用初始化列表初始化
}
}  // namespace n3

namespace n4 {
// 类型收窄指的是导致数据内容发生变化或者精度丢失的隐式类型转换。
// 类型收窄的几种情况：
// 1.从浮点数隐式转换为整型数，如int i=2.2;
// 2.从高精度浮点数隐式转换为低精度浮点数，如从long double转double或float。
// 3.从整型数隐式转换为浮点数，并且超出浮点数的表示范围，如float=(unsigned long Long)-1，
//   注意这表示将-1先强制转换为unsigned long long，再隐式转换为float。
// 4.从整型数隐式转换为长度较短的整型数，并且超出其表示范围。如char x=65536。
// 在C++98/03中，类型收窄编译器不会报错，而C++11中可以通过列表初始化来检查及防止类型收窄。

void func1() {
  int x1(5.3);   // ok，x1=5
  int x2 = 5.3;  // ok，x2=5
  // int x3{5.3};   // error：narrowing，gcc给的是warning
  // int x4 = {5.3};  // 同上

  char c1{7};  // ok
  // char c2{99999};  // error：narrowing，gcc给的是warning

  std::vector<int> v1{1, 2, 3, 4, 5};  // ok
  // std::vector<int> v2{1, 2, 3, 4, 5.6};  // error：narrowing，gcc给的是warning
}

void func2() {
  float f1 = 1e40;  // ok，double->float
  // float f2 = {1e40};  // error，double->float，超过float能表示的范围
  float f3 = (unsigned long long)-1;  // 将-1（0xFFFFFFFF）强转，再隐式转成float
  // float f4 = {(unsigned long long)-1};  // error
  float f5 = (unsigned long long)1;    // 将1强转，再隐式转成float
  float f6 = {(unsigned long long)1};  // ok
}

void func3() {
  const int x = 1024, y = 1;  // 注意x，y被const修饰
  char c1 = x;  // ok，overflow in implicit constant conversion [-Woverflow]
  // char c2 = {x};  // error
  char c3 = y;    // ok
  char c4 = {y};  // ok, 由于加了const并且值为1，编译器认为这样的转换是安全的

  int z = 1;
  char c5 = z;    // ok
  char c6 = {z};  // narrowing conversion of ‘z’ from ‘int’ to ‘char’
}
}  // namespace n4

int main() {}