#include <iostream>
#include <string>

// 尽可能延后变量定义式出现的时间

namespace n1 {
std::string encryptPassword(const std::string &password) {
  std::string encrypted;  // 过早定义的变量
  if (password.length() < 8) {
    // 此时encrypted没被使用：
    throw std::logic_error("Password is too short");
  }
  // 必要动作，处理encrypted
  return encrypted;
}
}  // namespace n1

namespace n2 {
std::string encryptPassword(const std::string &password) {
  if (password.length() < 8) {
    throw std::logic_error("Password is too short");
  }
  // 延后定义：
  std::string encrypted;  // 上面抛出异常时不会浪费构造和析构成本
  encrypted = password;
  // 必要动作，处理encrypted
  return encrypted;
}
// encrypted虽然定义却无任何实参作为初值，调用的是default构造函数。
// 条款4说通过default构造函数构造初对象，然后对它赋值比直接在构造时指定初值效率差。
}  // namespace n2

namespace n3 {
std::string encryptPassword3(const std::string &password) {
  if (password.length() < 8) {
    throw std::logic_error("Password is too short");
  }
  // 定义并初始化的最佳做法：
  std::string encrypted(password);
  return encrypted;
}
// 不止应该延后变量定义，直到非得使用该变量的前一刻为止，
// 甚至应该尝试延后这份定义直到能够给它初值实参为止。
// 如果这样，不仅能够避免构造和析构非必要对象，还可以避免无意义的default构造行为。
}  // namespace n3

namespace n4 {
class A {
 public:
  A(){};
  A(int) {}
  A(const A &) {}
};
// 如果变量只在循环内使用，把它定义于循环内好，
// 还是把它定义于循环外并在每次循环迭代时赋值给它比较好。
void func1() {
  A a;
  for (int i = 0; i < 5; ++i) {  // 定义于循环外
    // 成本：1个构造函数 + 1个析构函数 + n个赋值操作
    a = i;
  }
}
void func2() {
  for (int i = 0; i < 5; ++i) {  // 定义于循环内
    // 成本：n个构造函数+n个析构函数
    A a = i;
  }
}
// 如果class的一个赋值成本低于一组构造+析构成本，
// 定义在循环外比较高效，特别当n值很大的时候，否则定义在循环体内或许较好。
// 此外定义在循环体外，造成a的作用域比定义在循环内大，
// 有时候对程序的可理解性和易维护性造成冲突。

// 因此除非：
// 1.知道赋值成本比构造+析构成本低；
// 2.正在处理代码中效率高敏感的部分，否则应该定义在循环体内。
}  // namespace n4

// 请记住：
// 1.尽可能延后变量定义的出现，这样做可增加程序的清晰度并改善程序效率。

int main() { return 0; }