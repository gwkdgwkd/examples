#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>

// generate
// generate_n

namespace n1 {
// 用for_each()算法将一个函数对象应用到序列中的每一个元素上，
// 函数对象的参数是for_each()的前两个参数所指定序列中元素的引用，可以修改元素。
// generate()则不同，它的前两个参数是指定范围的正向迭代器，第三个参数是T fun();
// 这种没有参数的函数对象无法在for_each()内访问序列元素的值。
// generate()只会保存函数为序列中每个元素所返回的值，而且generate()没有返回值。
// 为了使这个算法更有用，可以将生成的不同的值赋给无参数函数中的不同元素。
// 也可以用一个可以捕获一个或多个外部变量的函数对象作为第三个参数。

void func() {
  std::string chars(30, ' ');
  char ch{'a'};
  int incr{};
  std::generate(std::begin(chars), std::end(chars), [ch, &incr] {
    incr += 3;
    return ch + (incr % 26);
  });
  std::cout << chars << std::endl;
  // dgjmpsvybehknqtwzcfiloruxadgjm
}
}  // namespace n1

namespace n2 {
// generate_n()和generate()的工作方式是相似的。
// 不同之处是，它的第一个参数仍然是序列的开始迭代器，
// 第二个参数是由第三个参数设置的元素的个数。
// 为了避免程序崩溃，这个序列必须至少有第二个参数定义的元素个数。

void func() {
  std::string chars(30, ' ');  // 30 space characters
  char ch{'a'};
  int incr{};
  std::generate_n(std::begin(chars), chars.size() / 2, [ch, &incr] {
    incr += 3;
    return ch + (incr % 26);
  });
  std::cout << '{' << chars << '}' << std::endl;
  // {dgjmpsvybehknqt               }
}
}  // namespace n2

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}