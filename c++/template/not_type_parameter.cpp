#include <algorithm>
#include <cstring>
#include <iostream>
#include <iterator>

// 模板是一种泛型技术，目的是将数据的类型参数化，
// 以增强C++语言（强类型语言）的灵活性。
// C++对模板的支持非常自由，除了可以包含类型参数，还可以包含非类型参数。
// 类型参数和非类型参数都可以用在函数体或者类体中。
// 当调用一个函数模板或者通过一个类模板创建对象时，
// 非类型参数会被用户提供的、或者编译器推断出的值所取代。

// 非类型参数的类型不能随意指定，它受到了严格的限制，
// 只能是一个整数，或者是一个指向对象或函数的指针（或引用）。
// 1.当非类型参数是一个整数时，传递给它的实参，
//   或者由编译器推导出的实参必须是一个常量表达式，
//   例如10、2*30、18+23-4等，但不能是n、n+10、n+m等（n和m都是变量）。
// 2.当非类型参数是一个指针（引用）时，
//   绑定到该指针的实参必须具有静态的生存期；
//   换句话说，实参必须存储在虚拟地址空间中的静态数据区。
//   局部变量位于栈区，动态创建的对象位于堆区，它们都不能用作实参。

namespace n1 {
int a[5] = {1, 2, 3, 4, 5};
int b[5] = {10, 20, 30, 40, 50};

template <typename T>
void Swap(T a[], T b[], int len) {
  // 为什么在函数内部不能求得数组长度，一定要通过形参把数组长度传递进去呢？
  // 这是因为数组在作为函数参数时会自动转换为数组指针，
  // 而sizeof只能通过数组名求得数组长度，不能通过数组指针求得数组长度。
  std::cout << "sizeof : " << sizeof(*a) << "," << sizeof(a) << std::endl;
  std::cout << "len : " << len << std::endl;
  // sizeof : 4,8
  // len : 5
}
void func1() {
  // 形参len用来指明要交换的数组的长度，
  // 调用Swap()函数之前必须先通过sizeof求得数组长度再传递给它：
  Swap(a, b, sizeof(a) / sizeof(int));
}

// 多出来的形参len给编码带来了不便，可以借助模板中的非类型参数将它消除：
template <typename T, unsigned N>
void Swap(T (&a)[N], T (&b)[N]) {
  T temp;
  for (int i = 0; i < N; i++) {
    temp = a[i];
    a[i] = b[i];
    b[i] = temp;
  }
}
template <typename T, unsigned N>
void printArray(T (&arr)[N]) {
  for (int i = 0; i < N; i++) {
    if (i == N - 1) {
      std::cout << arr[i] << std::endl;
    } else {
      std::cout << arr[i] << ",";
    }
  }
}
void func2() {
  printArray(a);  // 1, 2, 3, 4, 5
  printArray(b);  // 10, 20, 30, 40, 50
  Swap(a, b);  // 编译器用int数组来代替类型参数T，使用长度来代替非类型参数N
  printArray(a);  // 1, 2, 3, 4, 5
  printArray(b);  // 10, 20, 30, 40, 50

  // int len;
  // cin >> len;
  // int a[len];
  // int b[len];
  // Swap(a, b);  // 错误编译器推导出来的实参是len，是一个变量，而不是常量
}

void test() {
  func1();
  func2();
}
}  // namespace n1

namespace n2 {
// 在类模板中使用非类型参数：
// C/C++规定，数组一旦定义后，它的长度就不能改变了；
// 换句话说，数组容量不能动态地增大或者减小，
// 这样的数组称为静态数组（Static array）。
// 静态数组有时候会给编码代码不便，
// 可以通过自定义的Array类来实现动态数组（Dynamic array）。
// 所谓动态数组，是指数组容量能够在使用的过程中随时增大或减小。
template <typename T, int N>
class Array {
 public:
  Array();
  ~Array();

 public:
  T &operator[](int i);                    // 重载下标运算符[]
  int length() const { return m_length; }  // 获取数组长度
  bool capacity(int n);                    // 改变数组容量
 private:
  int m_length;    // 数组的当前长度
  int m_capacity;  // 当前内存的容量（能容乃的元素的个数）
  T *m_p;          // 指向数组内存的指针
};
template <typename T, int N>
Array<T, N>::Array() {
  m_p = new T[N];
  m_capacity = m_length = N;
}
template <typename T, int N>
Array<T, N>::~Array() {
  delete[] m_p;
}
template <typename T, int N>
T &Array<T, N>::operator[](int i) {
  if (i < 0 || i >= m_length) {
    std::cout << "Exception: Array index out of bounds!" << std::endl;
  }
  return m_p[i];
}
template <typename T, int N>
bool Array<T, N>::capacity(int n) {
  if (n > 0) {                // 增大数组
    int len = m_length + n;   // 增大后的数组长度
    if (len <= m_capacity) {  // 现有内存足以容纳增大后的数组
      m_length = len;
      return true;
    } else {  // 现有内存不能容纳增大后的数组
      T *pTemp =
          new T[m_length + 2 * n * sizeof(T)];  // 增加的内存足以容纳2*n个元素
      if (pTemp == NULL) {                      // 内存分配失败
        std::cout << "Exception: Failed to allocate memory!" << std::endl;
        return false;
      } else {  // 内存分配成功
        memcpy(pTemp, m_p, m_length * sizeof(T));
        delete[] m_p;
        m_p = pTemp;
        m_capacity = m_length = len;
        return true;
      }
    }
  } else {                        // 收缩数组
    int len = m_length - abs(n);  // 收缩后的数组长度
    if (len < 0) {
      std::cout << "Exception: Array length is too small!" << std::endl;
      return false;
    } else {
      m_length = len;
      return true;
    }
  }
}

void test() {
  Array<int, 5> arr;
  auto print = [&arr]() {
    for (int i = 0, len = arr.length(); i < len; i++) {
      std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
  };

  for (int i = 0, len = arr.length(); i < len; i++) {
    arr[i] = 2 * i;
  }
  print();  // 0 2 4 6 8

  arr.capacity(8);
  for (int i = 5, len = arr.length(); i < len; i++) {
    arr[i] = 2 * i;
  }
  print();  // 0 2 4 6 8 10 12 14 16 18 20 22 24

  arr.capacity(-4);
  print();  // 0 2 4 6 8 10 12 14 16

  // int len = 5;
  // Array<int, len> arr;
  // 错误，编译器推导出来的实参是len，是一个变量，而不是常量
}
}  // namespace n2

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::test();
      break;
    case 1:
      n2::test();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}