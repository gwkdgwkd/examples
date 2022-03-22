#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

using word_iter = std::vector<std::string>::iterator;
std::vector<std::string> words1{"one", "two",   "three", "four", "five",
                                "six", "seven", "eight", "nine"};
std::vector<std::string> words2{"two",   "three", "four", "five", "six",
                                "seven", "eight", "nine", "ten"};
auto iter1 = std::begin(words1);
auto end_iter1 = std::end(words1);
auto iter2 = std::begin(words2);
auto end_iter2 = std::end(words2);

// 可以用和比较字符串类似的方式来比较序列。
// 如果两个序列的长度相同，并且对应元素都相等，equal()算法会返回true。
// 有4个版本的equal()算法，其中两个用==运算符来比较元素，
// 另外两个用我们提供的作为参数的函数对象来比较元素，所有指定序列的迭代器都必须至少是输入迭代器。
// 尽管可以用equal()来比较两个同种类型的容器的全部内容，但最好还是使用容器的成员函数operator==()来做这些事。
// 不应该用equal()来比较来自于无序map或set容器中的元素序列。
// 在无序容器中，一组给定元素的顺序可能和保存在另一个无序容器中的一组相等元素不同，
// 因为不同容器的元素很可能会被分配到不同的格子中。
void func1() {
  std::cout << "Container - words1:";
  std::copy(iter1, end_iter1,
            std::ostream_iterator<std::string>{std::cout, " "});
  std::cout << "\nContainer - words2:";
  std::copy(iter2, end_iter2,
            std::ostream_iterator<std::string>{std::cout, " "});
  std::cout << std::endl;

  std::cout.setf(std::ios::boolalpha);
  std::cout << (words1 == words2) << std::endl;  // false

  std::cout << "\n1. Compare from words1[1] to end with words2: ";
  // 3个输入迭代器参数，前两个参数是第一个序列的开始和结束迭代器，第三个参数是第二个序列的开始迭代器。
  // 如果第二个序列中包含的元素少于第一个序列，结果是未定义的。
  std::cout << std::equal(iter1 + 1, end_iter1, iter2) << std::endl;

  std::cout << "2. Compare from words2[0] to second-to-last with words1:";
  std::cout << std::equal(iter2, end_iter2 - 1, iter1) << std::endl;

  std::cout << "3. Compare from words1[1] to words1[5] with words2:";
  // 当用equal()从开始迭代器开始比较两个序列时，第二个序列用来和第一个序列比较的元素个数由第一个序列的长度决定。
  // 就算第二个序列比第一个序列的元素多，equal()仍然会返回true。
  std::cout << std::equal(iter1 + 1, iter1 + 6, iter2) << std::endl;

  std::cout << "4. Compare first 6 from words1 with first 6 in words2:";
  // 4个输入迭代器参数，第一个序列的开始和结束迭代器，第二个序列的开始和结束迭代器，
  // 如果两个序列的长度不同，那么结果总是为false。
  // 推荐使用接受4个参数的版本，因为它不会产生未定义的行为。
  std::cout << std::equal(iter1, iter1 + 6, iter2, iter2 + 6) << std::endl;

  std::cout << "5. Compare all words1 with words2:";
  std::cout << std::equal(iter1, end_iter1, iter2) << std::endl;

  std::cout << "6. Compare all of words1 with all of words2:";
  std::cout << std::equal(iter1, end_iter1, iter2, end_iter2) << std::endl;

  std::cout << "7. Compare from words1[1] to end with words2 from first to "
               "second-to-last:";
  // 如果为两个序列提供了开始和结束迭代器，为了使结果为true，序列必须是相同的长度。
  std::cout << std::equal(iter1 + 1, end_iter1, iter2, end_iter2 - 1)
            << std::endl;

  // Container - words1: one two three four five six seven eight nine
  // Container - words2: two three four five six seven eight nine ten
  // 1.Compare from words1[1] to end with words2: true
  // 2.Compare from words2[0] to second-to-last with words1: false
  // 3.Compare from words1[1] to wordsl[5] with words2: true
  // 4.Compare first 6 from words1 with first 6 in words2: false
  // 5.Compare all wordsl with words2: false
  // 6.Compare all of words1 with all of words2: false
  // 7.Compare from words1[1] to end with words2 from first to second_to_last: true

  std::vector<std::string> r1{"three", "two", "ten"};
  std::vector<std::string> r2{"twelve", "ten", "twenty"};
  std::cout << std::equal(std::begin(r1), std::end(r1), std::begin(r2),
                          [](const std::string& s1, const std::string& s2) {
                            return s1[0] == s2[0];
                          })
            << std::endl;  // true
  std::cout << std::equal(std::begin(r1), std::end(r1), std::begin(r2),
                          std::end(r2),
                          [](const std::string& s1, const std::string& s2) {
                            return s1[0] == s2[0];
                          })
            << std::endl;  // true
}

// equal()算法可以告诉我们两个序列是否匹配。
// mismatch()算法也可以告诉我们两个序列是否匹配，而且如果不匹配，它还能告诉我们不匹配的位置。
// mismatch()的4个版本和equal()一样有相同的参数——第二个序列有或没有结束迭代器，有或没有定义比较的额外的函数对象参数。
// mismatch()返回的pair对象包含两个迭代器。
// 它的first成员是一个来自前两个参数所指定序列的迭代器，second是来自于第二个序列的迭代器。
// 当序列不匹配时，pair包含的迭代器指向第一对不匹配的元素；因此这个pair对象为pair<iter1+n，iter2+n>，
// 这两个序列中索引为n的元素是第一个不匹配的元素。
// 当序列匹配时，pair的成员取决于使用的mismatch()的版本和具体情况。
// iter1和end_iter1表示定义第一个序列的迭代器，iter2和end_iter2表示第二个序列的开始和结束迭代器。
// 返回的匹配序列的pair的内容如下：
//  对于mismatch(iter1，end_iter1，iter2):
//   返回pair<end_iter1,(iter2+(end_ter1-iter1))>，pair的成员second等于iter2加上第一个序列的长度。
//   如果第二个序列比第一个序列短，结果是未定义的。
//  对于mismatch(iterl, end_iter1, iter2, end_iter2)：
//   当第一个序列比第二个序列长时，返回pair<end_iter1,(iter2+(end_iter1-iter1))>，所以成员second为iter2加上第一个序列的长度。
//   当第二个序列比第一个序列长时，返回pair<(iter1+(end_iter2-iter2)),end_iter2>， 所以成员first等于iter1加上第二个序列的长度。
//   当序列的长度相等时，返回pair<end_iter1, end_iter2>。
// 不管是否添加一个用于比较的函数对象作为参数，上面的情况都同样适用。
void func2() {
  auto print_match = [](const std::pair<word_iter, word_iter>& pr,
                        const word_iter& end_iter) {
    if (pr.first != end_iter)
      std::cout << "\nFirst pair of words that differ are " << *pr.first
                << " and " << *pr.second << std::endl;
    else
      std::cout << "\nRanges are identical." << std::endl;
  };
  std::cout << "Container - words1:  ";
  std::copy(iter1, end_iter1,
            std::ostream_iterator<std::string>{std::cout, " "});
  std::cout << "\nContainer - words2:  ";
  std::copy(iter2, end_iter2,
            std::ostream_iterator<std::string>{std::cout, " "});
  std::cout << std::endl;

  std::cout << "\n1. Compare from words1[1] to end with words2:";
  print_match(std::mismatch(iter1 + 1, end_iter1, iter2), end_iter1);
  std::cout << "2. Compare from words2[0] to second-to-last with words1:";
  print_match(std::mismatch(iter2, end_iter2 - 1, iter1), end_iter2 - 1);
  std::cout << "3. Compare from words1[1] to words1[5] with words2:";
  print_match(std::mismatch(iter1 + 1, iter1 + 6, iter2), iter1 + 6);
  std::cout << "4.Compare first 6 from words1 with first 6 in words2:";
  print_match(std::mismatch(iter1, iter1 + 6, iter2, iter2 + 6), iter1 + 6);
  std::cout << "5. Compare all words1 with words2:";
  print_match(std::mismatch(iter1, end_iter1, iter2), end_iter1);
  std::cout << "6. Compare all of words2 with all of words1:";
  print_match(std::mismatch(iter2, end_iter2, iter1, end_iter1), end_iter2);
  std::cout
      << "7. Compare from words1[1] to end with words2[0] to second-to-last:";
  print_match(std::mismatch(iter1 + 1, end_iter1, iter2, end_iter2 - 1),
              end_iter1);

  // Container - words1:  one two three four five six seven eight nine
  // Container - words2:  two three four five six seven eight nine ten

  // 1. Compare from words1[1] to end with words2:
  // Ranges are identical.
  // 2. Compare from words2[0] to second-to-last with words1:
  // First pair of words that differ are two and one
  // 3. Compare from words1[1] to words1[5] with words2:
  // Ranges are identical.
  // 4.Compare first 6 from words1 with first 6 in words2:
  // First pair of words that differ are one and two
  // 5. Compare all words1 with words2:
  // First pair of words that differ are one and two
  // 6. Compare all of words2 with all of words1:
  // First pair of words that differ are two and one
  // 7. Compare from words1[1] to end with words2[0] to second-to-last:
  // Ranges are identical.

  std::vector<std::string> range1{"one", "three", "five", "ten"};
  std::vector<std::string> range2{"nine", "five", "eighteen", "seven"};
  auto pr = std::mismatch(std::begin(range1), std::end(range1),
                          std::begin(range2), std::end(range2),
                          [](const std::string& s1, const std::string& s2) {
                            return s1.back() == s2.back();
                          });
  if (pr.first == std::end(range1) || pr.second == std::end(range2)) {
    std::cout << "The ranges are identical." << std::endl;
  } else {
    std::cout << *pr.first << " is not equal to " << *pr.second << std::endl;
  }
  // five is not equal to eighteen
}

// lexicographical_compare()算法可以比较由开始和结束迭代器定义的两个序列。
// 它的前两个参数定义了第一个序列，第3和第4个参数分别是第二个序列的开始和结束迭代器。
// 默认用<运算符来比较元素，但在需要时，也可以提供一个实现小于比较的函数对象作为可选的第5个参数。
// 如果第一个序列的字典序小于第二个，这个算法会返回true，否则返回false。
// 所以，返回false表明第一个序列大于或等于第二个序列。
// 序列是逐个元素比较的。第一对不同的对应元素决定了序列的顺序。
// 如果序列的长度不同，而且短序列和长序列的初始元素序列匹配，那么短序列小于长序列。
// 长度相同而且对应元素都相等的两个序列是相等的。空序列总是小于非空序列。
// 对于任何类型的对象序列来说，字典序都是字母排序思想的泛化。
// 从两个序列的第一个元素开始依次比较对应的元素，前两个对象的不同会决定序列的顺序。
// 显然，序列中的对象必须是可比较的。
void func3() {
  std::vector<std::string> phrase1{"the", "tigers", "of", "wrath"};
  std::vector<std::string> phrase2{"the", "horses", "of", "instruction"};
  auto print = [](std::vector<std::string>& v1, std::vector<std::string> v2,
                  bool less) {
    std::cout << "[";
    std::copy(std::begin(v1), std::end(v1),
              std::ostream_iterator<std::string>{std::cout, " "});
    std::cout << "] ";
    std::cout << (less ? "are" : "are not") << " less than ";
    std::cout << "[";
    std::copy(std::begin(v2), std::end(v2),
              std::ostream_iterator<std::string>{std::cout, " "});
    std::cout << "]";
    std::cout << std::endl;
  };

  auto less =
      std::lexicographical_compare(std::begin(phrase1), std::end(phrase1),
                                   std::begin(phrase2), std::end(phrase2));
  print(phrase1, phrase2, less);
  // [the tigers of wrath ] are not less than [the horses of instruction ]

  less = std::lexicographical_compare(std::begin(phrase2), std::end(phrase2),
                                      std::begin(phrase1), std::end(phrase1));
  print(phrase2, phrase1, less);
  // [the horses of instruction ] are less than [the tigers of wrath ]

  less = std::lexicographical_compare(std::begin(phrase1), std::end(phrase1),
                                      std::begin(phrase1), std::end(phrase1));
  print(phrase1, phrase1, less);
  // [the tigers of wrath ] are not less than [the tigers of wrath ]

  // 可以在lexicographical_compare()调用中添加一个参数，得到相反的结果：
  less = std::lexicographical_compare(
      std::begin(phrase1), std::end(phrase1), std::begin(phrase2),
      std::end(phrase2), [](const std::string& s1, const std::string& s2) {
        return s1.length() < s2.length();
      });
  print(phrase1, phrase2, less);
  // [the tigers of wrath ] are less than [the horses of instruction ]
}

int main() {
  func1();
  func2();
  func3();
  return 0;
}