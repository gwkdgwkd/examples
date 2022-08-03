#include <iostream>

// 引用虽然是基于指针实现的，但它比指针更加易用，通过指针获取数据时需要加*，
// 书写麻烦，而引用不需要，它和普通变量的使用方式一样。
// C++的发明人Bjarne Stroustrup也说过，
// 他在C++中引入引用的直接目的是为了让代码的书写更加漂亮，
// 尤其是在运算符重载中，不借助引用有时候会使得运算符的使用很麻烦。

namespace n1 {
// 引用和指针的其他区别:

// 1.引用必须在定义时初始化，并且以后也要从一而终，不能再指向其他数据；
//   而指针没有这个限制，指针在定义时不必赋值，以后也能指向任意数据：
void func1() {
  int i = 4;
  int *p;
  p = &i;

  // int &r;  // ‘r’ declared as reference but not initialized
  int &r = i;
}

// 2.可以有const指针，但是没有const引用：
void func2() {
  int i = 6;

  const int *p1 = &i;  // 指向const变量的指针
  int const *p2 = &i;  // 指向const变量的指针
  int *const p3 = &i;  // const指针

  const int &r1 = i;  // 引用const变量
  int const &r2 = i;  // 引用const变量
  // 没有const引用，本来就不能改变指向，加上const是多此一举：
  // int &const r3 = i;
}

// 3.指针可以有多级，但是引用只能有一级：
void func3() {
  int i = 8;
  int *p = &i;
  int **pp = &p;  // 是合法的

  int &r = i;
  // int &&r = r;  // 不合法的，C++11中表示右值引用
  // 如果希望定义一个引用变量来指代另外一个引用变量，那么也只需要加一个&：
  int &rr = r;

  i = 2;
  std::cout << i << "," << r << "," << rr << std::endl;  // 2,2,2
}

// 4.指针和引用的自增（++）自减（--）运算意义不一样：
void func4() {
  int i[2] = {4, 6};
  int *p = &i[0];
  int &r = i[0];

  std::cout << *p << "," << r << std::endl;  // 4,4
  p++;  // 对指针使用++表示指向下一份数据
  r++;  // 对引用使用++表示它所指代的数据本身加1
  std::cout << *p << "," << r << std::endl;  // 6,5
}

void testN1() {
  func1();
  func2();
  func3();
  func4();
}
}  // namespace n1

namespace n2 {
void func1() {
  // 变量是要占用内存的，但是通过&r获取到的却不是引用变量的地址，
  // 而是被引用变量的地址：
  int a = 99;
  int &r = a;
  std::cout << a << ", " << r << std::endl;    // 99, 99
  std::cout << &a << ", " << &r << std::endl;  // 0x7ffe7c31f69c, 0x7ffe7c31f69c
}

int num = 99;
class A {
 public:
  A();
  void show() {
    std::cout << "n addr: " << std::hex << std::showbase << &n << std::endl;
    std::cout << "r addr: " << std::hex << std::showbase << &r << std::endl;
    std::cout << "p addr: " << std::hex << std::showbase << p << std::endl;
  }

 private:
  int n;
  int &r;
  int *p;
};
A::A() : n(0), r(num), p(&num) {}
void func2() {
  A *a = new A();
  std::cout << sizeof(A) << std::endl;
  std::cout << "a addr: " << std::hex << std::showbase << a << std::endl;
  std::cout << "num addr: " << &num << std::endl;
  a->show();

  // 24
  // a addr: 0x555e121652c0
  // num addr: 0x555e10cb5010
  // n addr: 0x555e121652c0
  // r addr: 0x555e10cb5010
  // p addr: 0x555e10cb5010

  // 从运行结果可以看出：
  // 1.r是占用内存的，如果不占用的话，sizeof(A)的结果应该为16;
  // 2.r与num地址是一样的。
  //   使用&r取地址时，编译器会对代码进行隐式的转换，
  //   使得代码输出的是r的内容（a的地址），而不是r的地址，
  //   这就是为什么获取不到引用变量的地址的原因。
  //   也就是说，不是变量r不占用内存，而是编译器不让获取它的地址。

  // 其实引用只是对指针进行了简单的封装，它的底层依然是通过指针实现的，
  // 引用占用的内存和指针占用的内存长度一样，
  // 在32位环境下是4个字节，在64位环境下是8个字节，
  // 之所以不能获取引用的地址，是因为编译器进行了内部转换，转换成指针。
  // 当引用作为形参时，也会有类似的转换。
}

void testN2() {
  func1();
  func2();
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
      n1::testN1();
      break;
    case 1:
      n2::testN2();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}