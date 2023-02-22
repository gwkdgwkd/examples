#include <array>
#include <iostream>

namespace n1 {
// constexpr是C++11标准新添加的，在此之前（C++98/03）只有const，
// 其在实际使用中经常会表现出两种不同的语义：
// 1.只读；
// 2.常量。

void func(const int x) {
  // x是一个只读的变量，其本质仍为变量，无法用来初始化array容器：
  // std::array<int, x> a1{1, 2, 3, 4, 5};
  // std::cout << a1[1] << std::endl;

  // c是一个只读变量的同时，还是一个值为5的常量，所以可以用来初始化array：
  const int c = 5;
  std::array<int, c> a2{1, 2, 3, 4, 5};
  std::cout << a2[1] << std::endl;
}

void func() { func(5); }

// C++11标准中，为了解决const关键字的双重语义问题：
// 1.保留了const表示只读的语义；
// 2.将常量的语义划分给了新添加的constexpr关键字。
// 因此C++11建议将const和constexpr的功能区分开，
// 即凡是表达只的场景都使用const，表达常量的场景都用constexpr。
}  // namespace n1

namespace n2 {
// 只读不就意味着其不能被修改吗？
// 答案是否定的，只读和不允许被修改之间并没有必然的联系。
void func() {
  int a = 10;
  // 用const修饰了cr变量，表示该变量只读，即无法通过变量自身去修改自己的值：
  const int &cr = a;
  std::cout << cr << std::endl;  // 10
  // 但这并不意味着cr的值不能借助其它变量间接改变：
  a = 20;
  std::cout << cr << std::endl;  // 20
}
}  // namespace n2

namespace n3 {
// 在大部分实际场景中，const和constexpr是可以混用的，
// 它们是完全等价的，都可以在程序的编译阶段计算出结果：
void func1() {
  const int a1 = 5 + 4;
  constexpr int a2 = 5 + 4;
}
// 但在某些场景中，必须明确使用constexpr：
constexpr int sqr1(int arg) { return arg * arg; }
const int sqr2(int arg) { return arg * arg; }
void func2() {
  // 只有常量才能初始化array容器：
  std::array<int, sqr1(10)> a1;  // 正确，因为sqr1时constexpr函数
  // std::array<int, sqr2(10)> a2;  // 错误，因为sqr2不是constexpr函数
}
void func() {
  func1();
  func2();
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