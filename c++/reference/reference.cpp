#include <iostream>

// 参数的传递本质上是一次赋值的过程，赋值就是对内存进行拷贝。
// 所谓内存拷贝，是指将一块内存上的数据复制到另一块内存上。
// 对于像char、bool、int、float等基本类型的数据，
// 它们占用的内存往往只有几个字节，对它们进行内存拷贝非常快速。
// 而数组、结构体、对象是一系列数据的集合，数据的数量没有限制，可能很少，也可能成千上万，
// 对它们进行频繁的内存拷贝可能会消耗很多时间，拖慢程序的执行效率。
// C/C++禁止在函数调用时直接传递数组的内容，而是强制传递数组指针。
// 而对于结构体和对象没有这种限制，调用函数时既可以传递指针，也可以直接传递内容；
// 为了提高效率，建议传递指针，这样做在大部分情况下并没有什么不妥。

// 在C++中，有了一种比指针更加便捷的传递聚合类型数据的方式，那就是引用（Reference）。
// 引用（Reference）是C++相对于C语言的又一个扩充。
// C++用的引用就如同C语言的指针一样重要，但它比指针更加方便和易用，有时候甚至是不可或缺的。
// 同指针一样，引用能够减少数据的拷贝，提高数据的传递效率。

namespace n1 {
// 引用可以看做是数据的一个别名，通过这个别名和原来的名字都能够找到这份数据。
// 引用必须在定义的同时初始化，并且以后也要从一而终，
// 不能再引用其它数据，这有点类似于常量（const变量）。
// 引用在定义时需要添加&，在使用时不能添加&，使用时添加&表示取地址。
void func1() {
  int a = 5;
  int &r = a;
  std::cout << a << " " << r << std::endl;  // 5 5
  r = 6;
  std::cout << a << " " << r << std::endl;  // 6 6
  a = 4;
  std::cout << a << " " << r << std::endl;  // 4 4

  std::cout << &a << " " << &r << std::endl;  // 0x7ffcea0293bc 0x7ffcea0293bc
}

void func2() {
  // 如果不希望通过引用来修改原始的数据，那么可以在定义时添加const限制，形式为：
  int a = 4;
  const int &b = a;
  int const &c = a;
  std::cout << a << "," << b << "," << c << std::endl;  // 4,4,4
  a = 3;
  std::cout << a << "," << b << "," << c << std::endl;  // 3,3,3

  // b = 6;  // assignment of read-only reference ‘b’
  // c = 7;  // assignment of read-only reference ‘c’
}

void testN1() {
  func1();
  func2();
}
}  // namespace n1

namespace n2 {
// 在定义或声明函数时，可以将函数的形参指定为引用的形式，
// 这样在调用函数时就会将实参和形参绑定在一起，让它们都指代同一份数据。
// 如此一来，如果在函数体中修改了形参的数据，那么实参的数据也会被修改，
// 从而拥有“在函数内部影响函数外部数据”的效果。
// 按引用传参在使用形式上比指针更加直观。
// 在的C++编程中，鼓励读者大量使用引用，
// 它一般可以代替指针（当然指针在C++中也不可或缺），C++标准库也是这样做的。
void swap1(int a, int b) {  // 直接传递参数内容
  int temp = a;
  a = b;
  b = temp;
}
void swap2(int *p1, int *p2) {  // 传递指针
  int temp = *p1;
  *p1 = *p2;
  *p2 = temp;
}
void swap3(int &r1, int &r2) {  // 按引用传参
  int temp = r1;
  r1 = r2;
  r2 = temp;
}

void testN2() {
  int a = 6;
  int b = 9;
  std::cout << a << "," << b << std::endl;  // 6,9
  swap1(a, b);
  std::cout << a << "," << b << std::endl;  // 6,9
  swap2(&a, &b);
  std::cout << a << "," << b << std::endl;  // 9,6
  swap3(a, b);
  std::cout << a << "," << b << std::endl;  // 6,9
}
}  // namespace n2

namespace n3 {
// 引用除了可以作为函数形参，还可以作为函数返回值：
int &func1(int &r) {
  r += 10;
  return r;
}

// 在将引用作为函数返回值时应该注意一个小问题，就是不能返回局部数据的引用，
// 因为当函数调用完成后局部数据就会被销毁，有可能在下次使用时数据就不存在了。
// C++编译器检测到该行为时也会给出警告：
int &func2(int &r) {
  int m = r + 10;
  return m;
  // warning: reference to local variable ‘m’ returned [-Wreturn-local-addr]
}

void testN3() {
  int i = 10;
  std::cout << func1(i) << std::endl;  // 20
  std::cout << func1(i) << std::endl;  // 30

  std::cout << func2(i) << std::endl;  // 崩溃
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
      n1::testN1();
      break;
    case 1:
      n2::testN2();
      break;
    case 2:
      n3::testN3();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}