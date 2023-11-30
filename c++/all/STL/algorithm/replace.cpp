#include <algorithm>
#include <deque>
#include <iostream>
#include <iterator>
#include <vector>

// replace
// replace_if
// replace_copy_if

namespace n1 {
// replace()算法会用新的值来替换和给定值相匹配的元素。
// 它的前两个参数是序列的正向迭代器，第3个参数是被替换的值，第4个参数是新的值。
void func() {
  std::deque<int> data{10, -5, 12, -6, 10, 8, -7, 10, 11};
  std::replace(std::begin(data), std::end(data), 10, 99);
  copy(data.begin(), data.end(), std::ostream_iterator<int>{std::cout, " "});
  std::cout << std::endl;  // 99 -5 12 -6 99 8 -7 99 11
}
}  // namespace n1

namespace n2 {
// replace_if()会将使谓词返回true的元素替换为新的值。
// 它的第3个参数是一个谓词，参数的类型一般是元素类型的const引用，
// const不是强制性的，但谓词不应该改变元素，第4个参数是新的值。
void func() {
  std::string password{"This is a good choice !"};
  std::replace_if(
      std::begin(password), std::end(password),
      [](char ch) { return std::isspace(ch); }, '_');
  std::cout << password << std::endl;  // This_is_a_good_choice_!
}
}  // namespace n2

namespace n3 {
// replace_copy()和replace()做的事是一样的，但结果会被保存到其他容器，
// 而不会改变原始序列，replace会修改原始序列。
// 它的前两个参数是输入序列的正向迭代器，第3个参数是输入序列的开始迭代器，
// 最后两个参数分别是要被替换的值和替换值。
void func() {
  std::vector<std::string> words{"one", "none", "two", "three", "none", "four"};
  std::vector<std::string> new_words;
  std::replace_copy(std::begin(words), std::end(words),
                    std::back_inserter(new_words), std::string{"none"},
                    std::string{"0"});

  std::copy(new_words.begin(), new_words.end(),
            std::ostream_iterator<std::string>{std::cout, " "});
  std::cout << std::endl;  // one 0 two three 0 four

  std::copy(words.begin(), words.end(),
            std::ostream_iterator<std::string>{std::cout, " "});
  std::cout << std::endl;  // one none two three none four
}
}  // namespace n3

namespace n4 {
// 可以在序列中有选择地替换元素的最后一个算法是replace_copy_if()，
// 它和replace_if()算法是相同的，但它的结果会被保存到另一个序列中。
// 它的前两个参数是输入序列的迭代器，第3个参数是输出序列的开始迭代器，
// 最后两个参数分别是谓词和替换值。
void func() {
  std::deque<int> data1{10, -5, 12, -6, 10, 8, -7, 10, 11};
  std::vector<int> data_copy;
  std::replace_copy_if(
      std::begin(data1), std::end(data1), std::back_inserter(data_copy),
      [](int value) { return value == 10; }, 99);

  std::copy(data1.begin(), data1.end(),
            std::ostream_iterator<int>{std::cout, " "});
  std::cout << std::endl;  // 10 -5 12 -6 10 8 -7 10 11

  std::copy(data_copy.begin(), data_copy.end(),
            std::ostream_iterator<int>{std::cout, " "});
  std::cout << std::endl;  // 99 -5 12 -6 99 8 -7 99 11
}
}  // namespace n4

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
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
    case 3:
      n4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}