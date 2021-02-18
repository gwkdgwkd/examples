#include <iostream>
#include <vector>

// 在C++98/03中的对象初始化方法有很多种,这些不同的初始化方法，都有各自的适用范围和作用。最关键的是，这些种类繁多的初始化方法，没有一种可以通用所有情况。
//初始化列表:
int i_arr[3] = {1, 2, 3};  // 普通数组
struct A {
  int x;
  struct B {
    int i;
    int j;
  } b;
} a = {1, {2, 3}};  // POD类型,简单来说，是可以直接使用memcpy复制的对象。
// POD全称PlainOldData。通俗的讲，一个类或结构体通过二进制拷贝后还能保持其数据不变，那么它就是一个POD类型。
// 对于普通数组和POD类型，C++98/03可以使用初始化列表（initializerList）进行初始化，但是这种初始化方式的适用性非常狭窄，只有这两种数据类型可以使用初始化列表。

//拷贝初始化（copy-initialization）:
int i = 0;
class Foo1 {
 public:
  Foo1(int) {}
} foo1 = 123;  //需要拷贝构造函数

//直接初始化（direct-initialization）:
int j(0);
Foo1 bar(123);

// 为了统一初始化方式，并且让初始化行为具有确定的效果，C++11中提出了列表初始化（List-initialization）的概念。
class Foo {
 public:
  Foo(int) {}

 private:
  Foo(const Foo &);
};

// 在C++11中，可以直接在变量名后面跟上初始化列表，来进行对象的初始化。在初始化时，{}前面的等于号是否书写对初始化行为没有影响。
// 变量名后面跟上初始化列表方法同样适用于普通数组和POD类型的初始化:
int i_arr1[3]{1, 2, 3};  //普通数组
struct A1 {
  int x;
  struct B {
    int i;
    int j;
  } b1;
} a1{1, {2, 3}};  // POD类型

// 列表初始化还可以直接使用在函数的返回值上，return语句就如同返回了一个Foo(123,321.0)。
struct Foo2 {
  Foo2(int, double) {}
};
Foo2 func(void) { return {123, 321.0}; }

// 在C++11中使用初始化列表是非常便利的。它不仅统一了各种对象的初始化方式，而且还使代码的书写更加简单清晰。

int main() {
  // 在C++11中，初始化列表的适用性被大大增加了。它现在可以用于任何类型对象的初始化:
  Foo a1(123);
  // Foo a2 = 123; // error: 'Foo::Foo(const Foo &)' is private
  // a3、a4使用了新的初始化方式来初始化对象，效果如同a1的直接初始化。
  // a3虽然使用了等于号，但它仍然是列表初始化，因此，私有的拷贝构造并不会影响到它。
  Foo a3 = {123};
  Foo a4{123};  // 直接在变量名后面跟上初始化列表,C++98/03所不具备的。
  // a5、a6是基本数据类型的列表初始化方式。
  int a5 = {3};
  int a6{3};  // 直接在变量名后面跟上初始化列表,C++98/03所不具备的。

  // new操作符等可以用圆括号进行初始化的地方，也可以使用初始化列表:
  // 指针a指向了一个new操作符返回的内存，通过初始化列表方式在内存初始化时指定了值为123。
  int *a = new int{123};
  // b则是对匿名对象使用列表初始化后，再进行拷贝初始化。
  double b = double{12.12};
  // 堆上动态分配的数组终于也可以使用初始化列表进行初始化了。
  int *arr = new int[3]{1, 2, 3};

  // 类型收窄：指的是导致数据内容发生变化或者精度丢失的隐式类型转换
  // 类型收窄的几种情况：
  // 1 从浮点数隐式转换为整型数，如int i=2.2;
  // 2 从高精度浮点数隐式转换为低精度浮点数。如从long double转double或float。
  // 3 从整型数隐式转换为浮点数，并且超出浮点数的表示范围，如float=(unsigned long Long)-1，注意这表示将-1先强制转换为unsigned long long，再隐式转换为float。
  // 4 从整型数隐式转换为长度较短的整型数，并且超出其表示范围。如char x=65536。
  // 在C++98/03中，类型收窄编译器不会报错，而C++11中可以通过列表初始化来检查及防止类型收窄。

  int x1(5.3);   // ok，x1=5
  int x2 = 5.3;  // ok，x2=5
  // int x3{5.3};   // error：narrowing，gcc给的是warning
  // int x4 = {5.3};  // 同上
  char c1{7};  // ok
  // char c2{99999};  // error：narrowing，gcc给的是warning
  std::vector<int> v1{1, 2, 3, 4, 5};  // ok
  // std::vector<int> v2{1, 2, 3, 4, 5.6};  // error：narrowing，gcc给的是warning

  float fa = 1e40;  // ok，double->float
  // float fb = {1e40};  // error，double->float，超过float能表示的范围
  float fc = (unsigned long long)-1;  // 将-1（0xFFFFFFFF）强转，再隐式转成float
  // float fd = {(unsigned long long)-1};  // error
  float fe = (unsigned long long)1;    // 将1强转，再隐式转成float
  float ff = {(unsigned long long)1};  // ok

  const int x = 1024, y = 1;  // 注意x，y被const修饰
  char c = x;  // ok，overflow in implicit constant conversion [-Woverflow]
  // char d = {x};  // error
  char e = y;  // ok
  // ok。如果y为int型，则该行会出错。但由于加了const并且值为1。编译器认为这样的转换是安全的。
  char f = {y};

  return 0;
}