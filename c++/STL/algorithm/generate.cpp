#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>

// 用for_each()算法将一个函数对象应用到序列中的每一个元素上。
// 函数对象的参数是for_each()的前两个参数所指定序列中元素的引用，因此它可以直接修改被保存的值。
// generate()算法和它有些不同，它的前两个参数是指定范围的正向迭代器，第三个参数是用来定义T fun();
// 这种形式的函数的函数对象无法在函数内访问序列元素的值。
// generate()算法只会保存函数为序列中每个元素所返回的值，而且genemte()没有任何返回值。
// 为了使这个算法更有用，可以将生成的不同的值赋给无参数函数中的不同元素。
// 也可以用一个可以捕获一个或多个外部变量的函数对象作为generate()的第三个参数。

// generate_n()和generate()的工作方式是相似的。
// 不同之处是，它的第一个参数仍然是序列的开始迭代器，第二个参数是由第三个参数设置的元素的个数。
// 为了避免程序崩溃，这个序列必须至少有第二个参数定义的元素个数。

int main() {
  std::string chars(30, ' ');
  char ch{'a'};
  int incr{};
  std::generate(std::begin(chars), std::end(chars), [ch, &incr] {
    incr += 3;
    return ch + (incr % 26);
  });
  std::cout << chars << std::endl;
  // dgjmpsvybehknqtwzcfiloruxadgjm

  std::string chars1(30, ' ');  // 30 space characters
  char ch1{'a'};
  int incr1{};
  std::generate_n(std::begin(chars1), chars1.size() / 2, [ch1, &incr1] {
    incr1 += 3;
    return ch1 + (incr1 % 26);
  });
  std::cout << '{' << chars1 << '}' << std::endl;
  // {dgjmpsvybehknqt               }

  return 0;
}