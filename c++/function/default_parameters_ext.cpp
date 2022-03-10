#include <iostream>

namespace n2 {
void func2(int a, int b = 10, int c = 36) {
  std::cout << a << "," << b << "," << c << std::endl;
}

void func3(int a, int b = 30, int c = 60) {
  std::cout << a << "," << b << "," << c << std::endl;
}

void func4() {
  func3(1);
  func3(1, 2);
  func3(1, 2, 3);
}
}  // namespace n2