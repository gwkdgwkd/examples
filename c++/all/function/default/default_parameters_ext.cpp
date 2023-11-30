#include <iostream>

namespace n2 {
void func_2(int a, int b = 10, int c = 36) {
  std::cout << a << "," << b << "," << c << std::endl;
}

void func_3(int a, int b = 30, int c = 60) {
  std::cout << a << "," << b << "," << c << std::endl;
}

void func_4() {
  func_3(1);
  func_3(1, 2);
  func_3(1, 2, 3);
}
}  // namespace n2