#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

// unique()算法可以在序列中原地移除重复的元素，这就要求被处理的序列必须是正向迭代器所指定的。
// 在移除重复元素后，它会返回一个正向迭代器作为新序列的结束迭代器。
// 可以提供一个函数对象作为可选的第三个参数，这个参数会定义一个用来代替==比较元素的方法。
// 通过覆盖来消除连续元素。
// 没有元素会从输入序列中移除;算法并没有方法去移除元素，因为它并不知道它们的具体上下文。
// 整个序列仍然存在。但是，无法保证新末尾之后的元素的状态。

int main() {
  std::vector<std::string> words{"one", "two", "two", "three",
                                 "two", "two", "two"};
  auto end_iter = std::unique(std::begin(words), std::end(words));
  std::copy(std::begin(words), end_iter,
            std::ostream_iterator<std::string>{std::cout, " "});
  std::cout << std::endl;  // one two three two

  std::copy(std::begin(words), words.end(),
            std::ostream_iterator<std::string>{std::cout, " "});
  std::cout << std::endl;  // one two three two two two two

  words.erase(end_iter, std::end(words));  // 在执行unique()后，最好截断序列
  std::copy(std::begin(words), std::end(words),
            std::ostream_iterator<std::string>{std::cout, " "});
  std::cout << std::endl;  // one two three two

  // 可以将unique()运用到字符串中的字符上：
  std::string text{"There's   no   air    in    spaaaaaace!"};
  text.erase(
      std::unique(std::begin(text), std::end(text),
                  [](char ch1, char ch2) { return ch1 == ' ' && ch1 == ch2; }),
      std::end(text));
  std::cout << text << std::endl;
  // There's no air in spaaaaaace!

  return 0;
}