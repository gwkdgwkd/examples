#include <algorithm>
#include <deque>
#include <iostream>
#include <iterator>

using namespace std;

// replace()算法会用新的值来替换和给定值相匹配的元素。它的前两个参数是被处理序列的正向迭代器，第3个参数是被替换的值，第4个参数是新的值。

// replace_if()会将使谓词返回true的元素替换为新的值。它的第3个参数是一个谓词，第4个参数是新的值。参数的类型一般是元素类型的const引用；const不是强制性的，但谓词不应该改变元素。

// replace_copy()算法和replace()做的事是一样的，但它的结果会被保存到另一个序列中，而不会改变原始序列。
// 它的前两个参数是输入序列的正向迭代器，第3个参数是输入序列的开始迭代器，最后两个参数分别是要被替换的值和替换值。

// 可以在序列中有选择地替换元素的最后一个算法是replace_copy_if()，它和replace_if()算法是相同的，但它的结果会被保存到另一个序列中。
// 它的前两个参数是输入序列的迭代器，第3个参数是输出序列的开始迭代器，最后两个参数分别是谓词和替换值。

int main() {
  std::deque<int> data{10, -5, 12, -6, 10, 8, -7, 10, 11};
  std::replace(std::begin(data), std::end(data), 10, 99);
  copy(data.begin(), data.end(), ostream_iterator<int>{std::cout, " "});
  cout << endl;  // 99 -5 12 -6 99 8 -7 99 11

  string password{"This is a good choice !"};
  std::replace_if(std::begin(password), std::end(password),
                  [](char ch) { return std::isspace(ch); }, '_');
  cout << password << endl;  // This_is_a_good_choice_!

  std::vector<string> words{"one", "none", "two", "three", "none", "four"};
  std::vector<string> new_words;
  std::replace_copy(std::begin(words), std::end(words),
                    std::back_inserter(new_words), string{"none"}, string{"0"});
  copy(new_words.begin(), new_words.end(),
       ostream_iterator<string>{std::cout, " "});
  cout << endl;  // one 0 two three 0 four

  std::deque<int> data1{10, -5, 12, -6, 10, 8, -7, 10, 11};
  std::vector<int> data_copy;
  std::replace_copy_if(std::begin(data1), std::end(data1),
                       std::back_inserter(data_copy),
                       [](int value) { return value == 10; }, 99);
  copy(data1.begin(), data1.end(), ostream_iterator<int>{std::cout, " "});
  cout << endl;  // 10 -5 12 -6 10 8 -7 10 11
  copy(data_copy.begin(), data_copy.end(),
       ostream_iterator<int>{std::cout, " "});
  cout << endl;  // 99 -5 12 -6 99 8 -7 99 11

  return 0;
}