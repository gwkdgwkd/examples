#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

using namespace std;

// next_permutation()会生成一个序列的重排列，它是所有可能的字典序中的下一个排列，默认使用<运算符来做这些事情。
// 它的参数为定义序列的迭代器和一个返回布尔值的函数，这个函数在下一个排列大于上一个排列时返回true，如果上一个排列是序列中最大的，它返回false，所以会生成字典序最小的排列。

// next_permutation()是按照字典升序的方式生成的排列。当我们想以降序的方式生成排列时，可以使用prev_permutation()。
// prev_permutation和next_permutation()一样有两个版本，默认使用<来比较元素。因为排列是以降序的方式生成的，所以算法大多数时候会返回true。当生成最大排列时，返回false。

// is_permutation()算法可以用来检查一个序列是不是另一个序列的排列，如果是，会返回true。

int main() {
  std::vector<int> range{1, 2, 3};
  do {
    std::copy(std::begin(range), std::end(range),
              std::ostream_iterator<int>{std::cout, " "});
    std::cout << std::endl;
  } while (std::next_permutation(std::begin(range), std::end(range)));
  // 1 2 3
  // 1 3 2
  // 2 1 3
  // 2 3 1
  // 3 1 2
  // 3 2 1
  // 当next_permutation()返回false时，循环结束，表明到达最小排列。这样恰好可以生成序列的全部排列，只是因为序列的初始排列为 1、2、3，这是排列集合中的第一个排列。

  std::vector<string> words2{"one", "two", "three"};
  do {
    std::copy(std::begin(words2), std::end(words2),
              std::ostream_iterator<string>{std::cout, " "});
    std::cout << std::endl;
  } while (std::next_permutation(std::begin(words2), std::end(words2)));
  cout << "====" << endl;
  // one two three
  // three one two
  // three two one
  // two one three
  // two three one

  // 有一种方法可以得到序列的全排列，就是使用next_permutation()得到的最小排列：
  std::vector<string> words{"one", "two", "three"};
  while (std::next_permutation(std::begin(words), std::end(words)))
    ;
  do {
    std::copy(std::begin(words), std::end(words),
              std::ostream_iterator<string>{std::cout, " "});
    std::cout << std::endl;
  } while (std::next_permutation(std::begin(words), std::end(words)));
  cout << "====" << endl;
  // one three two
  // one two three
  // three one two
  // three two one
  // two one three
  // two three one

  // 当排列中的每个元素都小于或等于它后面的元素时，它就是元素序列的最小排列，所以可以用min_element()来返回一个指向序列中最小元素的迭代器，
  // 然后用iter_swap()算法交换两个迭代器指向的元素，从而生成最小的排列，例如：
  std::vector<string> words1{"one", "two", "three"};
  for (auto iter = std::begin(words1); iter != std::end(words1) - 1; ++iter)
    std::iter_swap(iter, std::min_element(iter, std::end(words1)));
  do {
    std::copy(std::begin(words1), std::end(words1),
              std::ostream_iterator<string>{std::cout, " "});
    std::cout << std::endl;
  } while (std::next_permutation(std::begin(words1), std::end(words1)));
  cout << "====" << endl;
  // one three two
  // one two three
  // three one two
  // three two one
  // two one three
  // two three one

  // 在开始生成全排列之前，可以先生成一个原始容器的副本，然后在循环中改变它，从而避免到达最小排列的全部开销。
  std::vector<string> words3{"one", "two", "three"};
  auto words_copy = words3;  // Copy the original
  do {
    std::copy(std::begin(words3), std::end(words3),
              std::ostream_iterator<string>{std::cout, " "});
    std::cout << std::endl;
    std::next_permutation(std::begin(words3), std::end(words3));
  } while (words3 != words_copy);
  cout << "====" << endl;
  // one two three
  // three one two
  // three two one
  // two one three
  // two three one
  // one three two

  // 找出单词中字母的全部排列
  //   std::vector<string> words4;
  //   string word;
  //   while (true) {
  //     std::cout << "\nEnter a word, or Ctrl+z to end: ";
  //     if ((std::cin >> word).eof()) break;
  //     string word_copy{word};
  //     do {
  //       words4.push_back(word);
  //       std::next_permutation(std::begin(word), std::end(word));
  //     } while (word != word_copy);
  //     size_t count{}, max{8};
  //     for (const auto& wrd : words4)
  //       std::cout << wrd << ((++count % max == 0) ? '\n' : ' ');
  //     std::cout << std::endl;
  //     words4.clear();  // Remove previous permutations
  //   }

  // 可以为next_permutation()提供一个函数对象作为第三个参数，从而用这个函数对象定义的比较函数来代替默认的比较函数。
  //   std::vector<string> words5{"one", "two", "three"};
  //   do {
  //     std::copy(std::rbegin(words5), std::end(words5),
  //               std::ostream_iterator<string>{std::cout, " "});
  //     std::cout << std::endl;
  //   } while (std::next_permutation(std::begin(words5), std::end(wowords5rds),
  //                                  [](const string& s1, const strings s2) {
  //                                    return s1.back() < s2.back();
  //                                  }));

  std::vector<double> data{44.5, 22.0, 15.6};
  do {
    std::copy(std::begin(data), std::end(data),
              std::ostream_iterator<double>{std::cout, " "});
    std::cout << std::endl;
  } while (std::prev_permutation(std::begin(data), std::end(data)));
  // 44.5 22 15.6
  // 44.5 15.6 22
  // 22 44.5 15.6
  // 22 15.6 44.5
  // 15.6 44.5 22
  // 15.6 22 44.5

  std::vector<double> data1{44.5, 22.0, 15.6, 1.5};
  std::vector<double> data2{22.5, 44.5, 1.5, 15.6};
  std::vector<double> data3{1.5, 44.5, 15.6, 22.0};
  auto test = [](const std::vector<double>& d1, const std::vector<double>& d2) {
    std::copy(std::begin(d1), std::end(d1),
              std::ostream_iterator<double>{std::cout, " "});
    std::cout << (is_permutation(std::begin(d1), std::end(d1), std::begin(d2))
                      ? "is"
                      : "is not")
              << " a permutation of ";
    std::copy(std::begin(d2), std::end(d2),
              std::ostream_iterator<double>{std::cout, " "});
    std::cout << std::endl;
  };
  test(data1, data2);
  test(data1, data3);
  test(data3, data2);
  // 44.5 22 15.6 1.5 is not a permutation of 22.5 44.5 1.5 15.6
  // 44.5 22 15.6 1.5 is a permutation of 1.5 44.5 15.6 22
  // 1.5 44.5 15.6 22 is not a permutation of 22.5 44.5 1.5 15.6

  // 另一个版本的is_permutation()允许只用开始迭代器指定第二个序列。在这种情况下，第二个序列可以包含比第一个序列还要多的元素，但是只会被认为拥有第一个序列中的元素个数。
  // 然而，并不推荐使用它，因为如果第二个序列包含的元素少于第一个序列，会产生未定义的错误。(C++11只有这种方式吧？)
  std::vector<double> data4{44.5, 22.0, 15.6, 1.5};
  std::vector<double> data5{1.5, 44.5, 15.6, 22.0, 88.0, 999.0};
  std::copy(std::begin(data4), std::end(data4),
            std::ostream_iterator<double>{std::cout, " "});
  std::cout << (is_permutation(std::begin(data4), std::end(data4),
                               std::begin(data5))
                    ? "is"
                    : "is not")
            << " a permutation of ";
  std::copy(std::begin(data5), std::end(data5),
            std::ostream_iterator<double>{std::cout, " "});
  std::cout << std::endl;
  // 44.5 22 15.6 1.5 is a permutation of 1.5 44.5 15.6 22 88 999

  return 0;
}