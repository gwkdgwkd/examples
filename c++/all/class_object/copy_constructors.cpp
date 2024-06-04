#include <cstring>
#include <iostream>

// 对于计算机来说，拷贝是指用一份已有的、已存在的数据创建出新的数据，
// 最终的结果是多了一份相同的数据。
// 在C++中，拷贝并没有脱离它本来的含义，只是将这个含义进行了特化，
// 是指用已经存在的对象创建出一个新的对象。
// 从本质上讲，对象也是一份数据，因为它会占用内存。

// 严格来说，对象的创建包括两个阶段：
// 1.首先要分配内存空间
//   分配内存很好理解，就是在堆区、栈区或者全局数据区留出足够多的字节。
//   这个时候的内存还比较原始，没有被初始化，
//   它所包含的数据一般是零值或者随机值，没有实际的意义。
// 2.然后再进行初始化
//   初始化就是首次对内存赋值，让它的数据有意义，再次赋值不叫初始化。
//   初始化的时候还可以为对象分配其他的资源或者提前进行一些计算等。
//   说白了，初始化就是调用构造函数。

namespace n1 {
// 当以拷贝的方式初始化对象时会调用拷贝构造函数。
// 这里有两个关键点，分别是以拷贝的方式和初始化对象。
// 初始化对象是指为对象分配内存后第一次填充数据，这个过程会调用构造函数。
// 对象被创建后必须立即被初始化，换句话说，只要创建对象，就会调用构造函数。
// 初始化和赋值都是将数据写入内存中，并且从表面上看起来，
// 初始化在很多时候都是以赋值的方式来实现的，所以很容易混淆。

// 对于基本类型的数据，很少会区分初始化和赋值这两个概念，
// 即使将它们混淆，也不会出现什么错误。
// 但是对于类，它们的区别就非常重要了，因为初始化时会调用构造函数，
// 以拷贝的方式初始化时会调用拷贝构造函数，而赋值时会调用重载过的赋值运算符。

// 如果没有显式地定义拷贝构造函数，那么编译器会自动生成一个默认的拷贝构造函数。
// 默认的拷贝构造函数很简单，使用老对象的成员变量对新对象的成员变量一一赋值。
// 对于简单的类，默认拷贝构造函数是够用的，也没有必要再显式地定义拷贝构造函数。
// 但是当类持有其它资源时，如动态分配的内存、打开的文件、指向其他数据的指针、
// 网络连接等，默认拷贝构造函数就不能拷贝这些资源，必须显式地定义拷贝构造函数，
// 以完整地拷贝对象的所有数据。

class A {
 public:
  A() { std::cout << "A" << std::endl; }
  A(const A &) { std::cout << "A copy" << std::endl; }
  A &operator=(const A &) {
    std::cout << "operator=" << std::endl;
    return *this;
  }
};

// 具体有哪些情况是以拷贝的方式来初始化对象呢？
// 1.将其它对象作为实参，即使没有定义拷贝构造函数，也是有效的，因为会生成默认的；
// 2.在创建对象的同时赋值，这是最常见的以拷贝的方式初始化对象的情况，非常容易理解；
void func1() {
  // 调用普通的构造函数，将此称为普通初始化：
  A a1;  // A

  // 在定义的同时进行赋值叫做初始化，调用拷贝构造函数，这就是以拷贝的方式初始化：
  A a2 = a1;  // A copy
  A a3(a1);   // A copy

  // 初始化只能有一次，赋值可以有多次，
  // 定义完成以后再赋值就叫做赋值，不管在定义的时候有没有赋值：
  a2 = a1;  // operator=
  a2 = a3;  // operator=
}
// 3.函数的形参为类类型，调用时要将另外一个对象作为实参传递进来赋值给形参，
//   这也是以拷贝的方式初始化形参对象；
void func2(A a) {
  // 函数是一段可以重复使用的代码，
  // 只有等到真正调用函数时才会为局部数据（形参和局部变量）在栈上分配内存。
  // 形参a在定义函数时对象并没有被创建，
  // 只有等到调用函数时才会真正地创建a对象，并在栈上为它分配内存。
  // 而创建a对象，就是以拷贝的方式进行的。

  // 其实形参在定义时就分配了内存，但此时并没有初始化，只有调用时才会初始化。
  // 是这样还是上面那样？
}
void func2() {
  A a;       // A
  func2(a);  // A copy
}
// 4.函数返回值为类类型。
A fun() {
  A a;
  return a;
}
void func3() {
  A a = fun();

  // 理论上讲，会调用两次拷贝构造函数，一次是返回对象，另外一次是创建a对象。
  // 当函数的返回值为类类型时，return语句会返回一个对象，
  // 不过为了防止局部对象被销毁，也为了防止通过返回值修改原来的局部对象，
  // 编译器并不会直接返回这个对象，而是根据这个对象先创建出临时对象，再将临时对象返回。
  // 而创建临时对象的过程，就是以拷贝的方式进行的，会调用拷贝构造函数。
  // g++ copy_constructors.cpp -fno-elide-constructors
  // A
  // A copy
  // A copy

  // 在较老的编译器上，或许真的能够看到调用两次拷贝构造函数，如iPhone上的C/C++编译器。
  // 但是在现代编译器上，只会调用一次拷贝构造函数，或者一次也不调用，
  // 例如在VS2010下会调用一次拷贝构造函数，在GCC、Xcode下一次也不会调用。
  // 这是因为，现代编译器都支持返回值优化技术，会尽量避免拷贝对象，以提高程序运行效率。
  // g++ copy_constructors.cpp
  // A
}

void func() {
  func1();
  func2();
  func3();
}
}  // namespace n1

namespace n2 {
// 当以拷贝的方式初始化一个对象时，会调用一个特殊的构造函数，就是拷贝构造函数。
// 拷贝构造函数只有一个参数，它是当前类的引用，一般都是const引用。
// 1.为什么必须是当前类的引用呢？
//   如果拷贝构造函数的参数不是当前类的引用，而是对象，那么在调用拷贝构造函数时，
//   会将另外一个对象直接传递给形参，这本身就是一次拷贝，
//   会再次调用拷贝构造函数，这个过程会一直持续下去，没有尽头，陷入死循环。
//   只有当参数是当前类的引用时，才不会导致再次调用拷贝构造函数，
//   这不仅是逻辑上的要求，也是C++语法的要求。
// 2.为什么是const引用呢？
//   拷贝构造函数的目的是用其它对象的数据来初始化当前对象，
//   并没有期望更改其它对象的数据，添加const限制后，这个含义更加明确了。
//   另外一个原因是，添加const后，可以将const对象和非const对象传递给形参，
//   否则只能把非const对象传给形参，因为非const类型可以转换为const类型，
//   而const类型不能转换为非const类型。

struct A {
  A() { std::cout << "A" << std::endl; }
  // A(A a) {} // 编译不过
  A(A &a) { std::cout << "A copy" << std::endl; }
};
void func1() {
  A a1;        // A
  const A a2;  // A
  A a3 = a1;   // A copy
  // A a4 = a2;
  // A a5(a2);
}

struct B {
  B() { std::cout << "B" << std::endl; }
  B(const B &a) { std::cout << "B copy" << std::endl; }
};
void func2() {
  B b1;        // B
  const B b2;  // B
  B b3 = b1;   // B copy
  B b4 = b2;   // B copy
  B b5(b2);    // B copy
}

// 一个类可以同时存在两个拷贝构造函数：
// 1.参数为const引用；
// 2.参数为非const引用。
struct C {
  C() { std::cout << "C" << std::endl; }
  C(C &a) { std::cout << "C copy" << std::endl; }
  C(const C &a) { std::cout << "const C copy" << std::endl; }
};
void func3() {
  C c1;        // C
  const C c2;  // C
  C c3 = c1;   // C copy
  C c4 = c2;   // const C copy
  C c5(c1);    // C copy
}

void func() {
  func1();
  func2();
  func3();
}
}  // namespace n2

namespace n3 {
// C++深拷贝和浅拷贝：
// 对于基本类型的数据以及简单的对象，它们的拷贝非常简单，就是按位复制。
// 这种默认的拷贝行为就是浅拷贝，这和调用memcpy()函数的效果非常类似。
// 对于简单的类，默认的拷贝构造函数就够用了，没必要再显式地定义类似的拷贝构造函数。
// 但是当类持有其它资源时，例如动态分配的内存、指向其他数据的指针等，
// 默认的拷贝构造函数就不能拷贝这些资源了，必须显式地定义拷贝构造函数，
// 以完整地拷贝对象的所有数据，这种将对象所持有的其它资源一并拷贝的行为叫做深拷贝，
// 必须显式地定义拷贝构造函数才能达到深拷贝的目的。
// 标准模板库（STL）中的string、vector、stack、set、map等都必须使用深拷贝。
// 到底是浅拷贝还是深拷贝？
// 如果一个类拥有指针类型的成员变量，那么绝大部分情况下就需要深拷贝，因为只有这样，
// 才能将指针指向的内容再复制出一份来，让原有对象和新生对象相互独立，彼此不受影响。
// 如果类的成员变量没有指针，一般浅拷贝足以。
// 另外一种需要深拷贝的情况就是在创建对象时进行一些预处理工作，
// 比如统计创建过的对象的数目、记录对象创建的时间等。

class Array {
 public:
  Array(int len);
  Array(const Array &arr);  // 拷贝构造函数
  ~Array();

 public:
  int operator[](int i) const { return m_p[i]; }  // 获取元素（读取）
  int &operator[](int i) { return m_p[i]; }       // 获取元素（写入）
  int length() const { return m_len; }

 private:
  int m_len;
  int *m_p;
};
Array::Array(int len) : m_len(len) { m_p = (int *)calloc(len, sizeof(int)); }
Array::Array(const Array &arr) {  // 拷贝构造函数
  this->m_len = arr.m_len;
  this->m_p = (int *)calloc(this->m_len, sizeof(int));
  memcpy(this->m_p, arr.m_p, m_len * sizeof(int));
}
Array::~Array() { free(m_p); }

// printArray()函数的形参为引用类型，这样做能够避免在传参时调用拷贝构造函数。
// printArray()函数不会修改任何数组元素，所以添加了const限制，以使得语义更加明确。
void printArray(const Array &arr) {
  int len = arr.length();
  for (int i = 0; i < len; i++) {
    if (i == len - 1) {
      std::cout << arr[i] << std::endl;
    } else {
      std::cout << arr[i] << ", ";
    }
  }
}

void func() {
  Array arr1(10);
  for (int i = 0; i < 10; i++) {
    arr1[i] = i;
  }
  Array arr2 = arr1;
  arr2[5] = 100;
  arr2[3] = 29;
  printArray(arr1);  // 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
  printArray(arr2);  // 0, 1, 2, 29, 4, 100, 6, 7, 8, 9

  // 拷贝构造函数删除后的打印：
  // 0, 1, 2, 29, 4, 100, 6, 7, 8, 9
  // 0, 1, 2, 29, 4, 100, 6, 7, 8, 9
  // free(): double free detected in tcache 2
}
}  // namespace n3

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
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
    case 2:
      n3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}