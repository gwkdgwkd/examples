#include <cstring>
#include <iostream>

// 在定义的同时进行赋值叫做初始化；
// 定义完成以后再赋值（不管在定义的时候有没有赋值）就叫做赋值。
// 初始化只能有一次，赋值可以有多次。
// 当以拷贝的方式初始化一个对象时，会调用拷贝构造函数；
// 当给一个对象赋值时，会调用重载的赋值运算符。

// C++拷贝控制操作（三/五法则）
// 当定义一个类时，显式地或隐式地指定了对象在拷贝、赋值和销毁时做什么。
// 一个类通过定义三种特殊的成员函数来控制这些操作，
// 分别是拷贝构造函数、赋值运算符和析构函数：
// 1.拷贝构造函数定义了当用同类型的另一个对象初始化新对象时做什么；
// 2.赋值运算符定义了将一个对象赋予同类型的另一个对象时做什么；
// 3.析构函数定义了此类型的对象销毁时做什么。
// 将这些操作称为拷贝控制操作。

// 由于拷贝控制操作是由三个特殊的成员函数来完成的，所以称此为C++三法则。
// 在C++11标准中，为了移动语义，又增加了移动构造函数和移动赋值运算符，
// 这样共有五个特殊的成员函数，所以又称为C++五法则。
// 三法则是针对较旧的C++89标准说的，五法则是针对较新的C++11标准说的。
// 为了统一称呼，后来人们干把它叫做C++三/五法则。
// 如果一个类没有定义所有这些拷贝控制成员，
// 编译器会自动为它定义默认的操作，因此很多类会忽略这些拷贝控制操作。
// 但是，对于一些持有其他资源的类来说，依赖这些默认的操作会导致灾难，
// 必须显式的定义这些操作。

// C++并不要求定义所有的这些操作，可以只定义其中的一个或两个。
// 但是，这些操作通常应该被看做一个整体，只需要定义其中一个操作，
// 而不需要定义其他操作的情况很少见。
// 当决定是否要为一个类显式地定义拷贝构造函数和赋值运算符时，
// 一个基本原则是首先确定这个类是否需要一个析构函数。
// 通常，对析构函数的需求要比拷贝构造函数和赋值运算符的需求更加明显。
// 如果一个类需要定义析构函数，
// 那么几乎可以肯定这个类也需要一个拷贝构造函数和一个赋值运算符。
// 如果为Array定义了一个析构函数，但却使用默认的拷贝构造函数和赋值运算符，
// 那么将导致不同对象之间相互干扰，修改一个对象的数据会影响另外的对象，
// 此还可能会导致内存操作错误：
// Array func(Array arr) {  // 按值传递，将发生拷贝
//   Array ret = arr;       // 发生拷贝
//   return ret;            // ret和arr将被销毁
// }
// 当func()返回时，arr和ret都会被销毁，在两个对象上都会调用析构函数，
// 此析构函数会free()掉m_p成员所指向的动态内存。
// 但是，这两个对象的m_p成员指向的是同一块内存，所以该内存会被free()两次，
// 这显然是一个错误，将要发生什么是未知的。
// func()的调用者还会继续使用传递给func()的对象：
// Array arr1(10);
// func(arr1);  // 当func()调用结束时，arr1.m_p指向的内存被释放
// Array arr2 = arr1;  // 现在arr2和arr1都指向无效内存

// 总之：
// 1.如果一个类需要定义析构函数，
//   那么几乎可以肯定它也需要定义拷贝构造函数和赋值运算符；
// 2.如果类需要拷贝构造函数，几乎可以肯定它也需要一个赋值运算符；
// 3.反之亦然；
// 4.无论需要拷贝构造函数还是需要赋值运算符，都不必然意味着也需要析构函数。

template <typename A>
void printArray(const A &arr) {
  int len = arr.length();
  for (int i = 0; i < len; i++) {
    if (i == len - 1) {
      std::cout << arr[i] << std::endl;
    } else {
      std::cout << arr[i] << ", ";
    }
  }
}

namespace n1 {
// 即使没有显式的重载赋值运算符，编译器也会以默认地方式重载它。
// 默认重载的赋值运算符很简单，就是将原有对象的所有成员变量赋值给新对象，
// 这和默认拷贝构造函数的功能类似。

class Array {
 public:
  Array(int len) : m_len(len) { m_p = (int *)calloc(len, sizeof(int)); }
  ~Array() { free(m_p); }

 public:
  int operator[](int i) const { return m_p[i]; }
  int &operator[](int i) { return m_p[i]; }
  int length() const { return m_len; }

 private:
  int m_len;
  int *m_p;
};

void func() {
  Array arr1(10);
  for (int i = 0; i < 10; i++) {
    arr1[i] = i;
  }
  printArray(arr1);  // 0, 1, 2, 3, 4, 5, 6, 7, 8, 9

  Array arr2(5);
  for (int i = 0; i < 5; i++) {
    arr2[i] = i;
  }
  printArray(arr2);  // 0, 1, 2, 3, 4

  arr2 = arr1;       // 调用默认的operator=()
  printArray(arr2);  // 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
  arr2[3] = 234;     // 修改arr2的数据会影响arr1
  arr2[7] = 920;

  printArray(arr2);  // 0, 1, 2, 234, 4, 5, 6, 920, 8, 9
  printArray(arr1);  // 0, 1, 2, 234, 4, 5, 6, 920, 8, 9

  // 由于调用了默认的赋值操作运算符，
  // arr1和arr2的m_p执向同一块内存，free时导致崩溃：
  // free(): double free detected in tcache 2
}
}  // namespace n1

namespace n2 {
class Array {
 public:
  Array() { std::cout << "Array" << std::endl; }
  Array(const Array &a) { std::cout << "Array copy" << std::endl; }

 public:
  Array operator=(const Array &arr) {  // 返回的不是引用，而是对象
    std::cout << "operator=" << std::endl;
    return *this;
  }
};

void func() {
  Array a1;  // Array
  Array a2;  // Array

  // 由于赋值操作符返回的不是引用，所以还要调用一次拷贝构造函数：
  a2 = a1;
  // operator=
  // Array copy
}
}  // namespace n2

namespace n3 {
class Array {
 public:
  Array() { std::cout << "Array" << std::endl; }
  Array(const Array &a) { std::cout << "Array copy" << std::endl; }

 public:
  Array &operator=(Array arr) {  // 参数是对象，不是引用
    std::cout << "operator=" << std::endl;
    return *this;
  }
};

void func() {
  Array a1;  // Array
  Array a2;  // Array

  // 参数是对象，不是引用，所以需要调用拷贝构造函数：
  a2 = a1;
  // Array copy
  // operator=
}
}  // namespace n3

namespace n4 {
class Array {
 public:
  Array() { std::cout << "Array" << std::endl; }
  Array(const Array &a) { std::cout << "Array copy" << std::endl; }

 public:
  Array &operator=(Array &arr) {  // 参数是引用，但不是常引用
    std::cout << "operator=" << std::endl;
    return *this;
  }
};

void func() {
  Array a1;        // Array
  const Array a2;  // Array
  Array a3;        // Array

  a3 = a1;  // operator=
  // 由于赋值操作符不是常量引用，所以不能用a2给a3赋值：
  // a3 = a2;
}
}  // namespace n4

namespace n5 {
class Array {
 public:
  Array() { std::cout << "Array" << std::endl; }
  Array(const Array &a) { std::cout << "Array copy" << std::endl; }

 public:
  Array &operator=(const Array &arr) {  // 参数是常引用
    std::cout << "operator=" << std::endl;
    return *this;
  }
};

void func() {
  Array a1;        // Array
  const Array a2;  // Array
  Array a3;        // Array

  a3 = a1;  // operator=
  a3 = a2;  // operator=
}
}  // namespace n5

namespace n6 {
class Array {
 public:
  Array(int len);
  Array(const Array &arr);  // 拷贝构造函数
  ~Array();

 public:
  int operator[](int i) const { return m_p[i]; }  // 获取元素（读取）
  int &operator[](int i) { return m_p[i]; }       // 获取元素（写入）
  Array &operator=(const Array &arr);             // 重载赋值运算符
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

// 赋值操作符注意事项：
// 1.operator=()的返回值类型为Array &，
//   即避免在返回数据时调用拷贝构造函数，还能够达到连续赋值的目的；
// 2.if(this!=&arr)判断是否是给同一个对象赋值：
//   如果是，那就什么也不做；
//   如果不是，那就将原有对象的所有成员变量赋值给新对象，并重新分配内存；
// 3.return *this表示返回当前对象（新对象）；
// 4.operator=()的形参类型为const Array&，
//   这样不但能够避免在传参时调用拷贝构造函数，
//   还能够同时接收const类型和非const类型的实参；
// 5.赋值运算符重载函数除了能有对象引用这样的参数之外，也能有其它参数。
//   但是其它参数必须给出默认值，例如：
//   Array & operator=(const Array &arr, int a = 100); 好像不行啊
Array &Array::operator=(const Array &arr) {
  if (this != &arr) {  // 判断是否是给自己赋值
    this->m_len = arr.m_len;
    free(this->m_p);  // 释放原来的内存
    this->m_p = (int *)calloc(this->m_len, sizeof(int));
    memcpy(this->m_p, arr.m_p, m_len * sizeof(int));
  }
  return *this;
}

void func() {
  Array arr1(10);
  for (int i = 0; i < 10; i++) {
    arr1[i] = i;
  }
  printArray(arr1);  // 0, 1, 2, 3, 4, 5, 6, 7, 8, 9

  Array arr2(5);
  for (int i = 0; i < 5; i++) {
    arr2[i] = i;
  }
  printArray(arr2);  // 0, 1, 2, 3, 4

  arr2 = arr1;       // 调用operator=()
  printArray(arr2);  // 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
  arr2[3] = 234;     // 修改arr2的数据不会影响arr1
  arr2[7] = 920;

  printArray(arr2);  // 0, 1, 2, 234, 4, 5, 6, 920, 8, 9
  printArray(arr1);  // 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
}
}  // namespace n6

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 5]" << std::endl;
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
    case 3:
      n4::func();
      break;
    case 4:
      n5::func();
      break;
    case 5:
      n6::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}
