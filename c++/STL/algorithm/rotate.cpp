#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

// rotate
// rotate_copy

// rotate()算法会从左边旋转序列的元素。
// rotate()的第一个参数是这个序列的开始迭代器；
// 第二个参数是指向新的第一个元素的迭代器，它必定在序列之内。
// 第三个参数是这个序列的结束迭代器。
// rotate()返回的迭代器指向之前的第一个元素。
void func1() {
  std::vector<std::string> words{"one",  "two", "three", "four",
                                 "five", "six", "seven", "eight"};
  auto iter =
      std::rotate(std::begin(words), std::begin(words) + 3, std::end(words));
  std::copy(std::begin(words), std::end(words),
            std::ostream_iterator<std::string>{std::cout, " "});
  std::cout << std::endl
            << "First element before rotation: " << *iter << std::endl;
  // four five six seven eight one two three
  // First element before rotation: one
}
void func2() {
  // 不需要对容器中的全部元素进行旋转：
  std::vector<std::string> words{"one", "two",   "three", "four", "five",
                                 "six", "seven", "eight", "nine", "ten"};
  auto start = std::find(words.begin(), std::end(words), "two");
  auto end = std::find(std::begin(words), std::end(words), "eight");
  auto iter = std::rotate(
      start, std::find(std::begin(words), std::end(words), "five"), end);
  std::copy(std::begin(words), std::end(words),
            std::ostream_iterator<std::string>{std::cout, " "});
  std::cout << std::endl
            << "First element before rotation: " << *iter << std::endl;
  // one five six seven two three four eight nine ten
  // First element before rotation : two
}

// rotate_copy()算法会在新序列中生成一个序列的旋转副本，并保持原序列不变。
// rotate_copy()的前3个参数和copy()是相同的；
// 第4个参数是一个输出迭代器，它指向目的序列的第一个元素。
// 这个算法会返回一个目的序列的输出迭代器，
// 它指向最后一个被复制元素的下一个位置。
// rotate_copy()返回的迭代器是words_copy中元素的结束迭代器。
void func3() {
  std::vector<std::string> words{"one", "two",   "three", "four", "five",
                                 "six", "seven", "eight", "nine", "ten"};
  auto start = std::find(std::begin(words), std::end(words), "two");
  auto end = std::find(std::begin(words), std::end(words), "eight");
  std::vector<std::string> words_copy;
  std::rotate_copy(start, std::find(std::begin(words), std::end(words), "five"),
                   end, std::back_inserter(words_copy));
  std::copy(std::begin(words), std::end(words),
            std::ostream_iterator<std::string>{std::cout, " "});
  std::cout << std::endl;
  std::copy(std::begin(words_copy), std::end(words_copy),
            std::ostream_iterator<std::string>{std::cout, " "});
  std::cout << std::endl;
  // one two three four five six seven eight nine ten
  // five six seven two three four
}
void func4() {
  // rotate_copy()返回的迭代器是words_copy中元素的结束迭代器：
  std::vector<std::string> words{"one", "two",   "three", "four", "five",
                                 "six", "seven", "eight", "nine", "ten"};
  auto start = std::find(std::begin(words), std::end(words), "two");
  auto end = std::find(std::begin(words), std::end(words), "eight");
  std::vector<std::string> words_copy{20};  // vector with 20 default elements
  auto end_copy_iter = std::rotate_copy(
      start, std::find(std::begin(words), std::end(words), "five"), end,
      std::begin(words_copy));
  std::copy(std::begin(words_copy), end_copy_iter,
            std::ostream_iterator<std::string>{std::cout, " "});
  std::cout << std::endl;
  // five six seven two three four
  // rotate_copy()算法现在会将现有元素的旋转序列保存到words_copy中。
  // 在输出时，这个算法返回的迭代器可以用来确定words_copy的尾部边界；
  // 如果没有它，就必须通过源序列的元素个数来计算出尾部边界。
}

int main() {
  func1();
  func2();
  func3();
  func4();
  return 0;
}