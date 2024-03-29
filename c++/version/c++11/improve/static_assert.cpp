#include <cassert>
#include <cstring>
#include <iostream>

// C++现有的标准中就有assert、#error两个方法是用来检查错误的，
// 除此而外还有一些第三方的静态断言实现。

// assert是运行期断言，它用来发现运行期间的错误，不能提前到编译期发现，
// 也不具有强制性，也谈不上改善编译信息的可读性。
// 既然是运行期检查，对性能肯定是有影响的，所以经常在发行版本中，assert都会被关掉。

// #error可看作是预编译期断言（甚至都算不上断言），仅仅能在预编译时显示信息，
// 可以配合#ifdef/ifndef参与预编译的条件检查，
// 由于它无法获得编译信息，当然，也就做不了更进一步分析了。

// 在stastic_assert提交到C++0x标准之前，为了弥补assert和#error的不足，出现了第三方方案，
// 可以作编译期的静态检查：BOOST_STATIC_ASSERT和LOKI_STATIC_CHECK，
// 但由于它们都是利用了一些编译器的隐晦特性实现的trick，
// 可移植性、简便性都不是太好，还会降低编译速度，而且功能也不够完善。
// 比如BOOST_STATIC_ASSERT就不能定义错误提示文字，
// 而LOKI_STATIC_CHECK则要求提示文字满足C++类型定义的语法。

// C++11中引入了static_assert这个关键字，用来做编译期间的断言，也叫作静态断言。
// static_assert(常量表达式，"提示字符串")
// 如果第一个参数常量表达式的值为false，会产生一条编译错误。
// 错误位置就是该static_assert语句所在行，第二个参数就是错误提示字符串。
// 使用static_assert，可以在编译期发现更多的错误，
// 用编译器来强制保证一些契约，帮助改善编译信息的可读性，尤其是用于模板时。

// 注意事项：
// 1.使用范围：
//   static_assert可以用在全局作用域中，命名空间中，
//   类作用域中，函数作用域中，几乎可以不受限制的使用。
// 2.常量表达式：
//   static_assert的断言表达式的结果必须是在编译时期可以计算的表达式，即必须是常量表达式：
static_assert(sizeof(int) == 4, "64-bit code generation is not supported.");
//   该static_assert用来确保编译仅在32位的平台上进行，不支持64位的平台，
//   该语句可放在文件的开头处，这样可以尽早检查，以节省失败情况下耗费的编译时间。
// 3.模板参数：
//   编译器在遇到一个static_assert时，通常立刻将其第一个参数作为常量表达式计算。
//   但如果该常量表达式依赖于某些模板参数，则延迟到模板实例化时再进行演算，
//   这就让检查模板参数也成为了可能：
template <typename T, typename U>
void bit_copy(T& a, U& b) {
  assert(sizeof(b) == sizeof(a));
  static_assert(sizeof(b) == sizeof(a), "template parameter size no equal!");
  memcpy(&a, &b, sizeof(b));
};
// 4.性能方面：
//   由于static_assert是编译期间断言，不生成目标代码，
//   因此static_assert不会造成任何运行期性能损失。

int main() {
  int varA = 0x2468;
  int varB = 5;
  bit_copy(varA, varB);

  double varC;
  bit_copy(varA, varC);
  // static_assert(sizeof(b) == sizeof(a), "template parameter size no equal!");

  return 0;
}

// g++ -std=c++11 static_assert.cpp
// error: static assertion failed: template parameter size no equal!