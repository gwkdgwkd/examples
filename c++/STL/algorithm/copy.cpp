#include <algorithm>
#include <deque>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_set>
#include <vector>

std::vector<int> v{1, 2, 3, 4, 5, 6, 7};
std::unordered_set<int> us{8, 9, 10};
void func1() {  // copy
  std::copy(us.begin(), us.end(), std::ostream_iterator<int>{std::cout, " "});
  std::cout << std::endl;  // 10 9 8
}

// copy_n()算法可以从源容器复制指定个数的元素到目的容器中。
// 第一个参数是指向第一个源元素的输入迭代器，
// 第二个参数是需要复制的元素的个数，
// 第三个参数是指向目的容器的第一个位置的迭代器。
// 这个算法会返回一个指向最后一个被复制元素的后一个位置的迭代器，
// 或者只是第三个参数——输出迭代器——如果第二个参数为0。
// 注意，如果被复制元素的个数超过了实际元素的个数，程序会因此崩溃。
// 如果元素的个数为0或负数，copy_n()算法什么也不做。
void func2() {
  std::copy_n(v.rbegin() + 1, 3, std::inserter(us, std::begin(us)));
  std::copy_n(us.begin(), us.size() - 1,
              std::ostream_iterator<int>{std::cout, " "});
  std::cout << std::endl;  // 4 5 6 8 9 10
}

// copy_if()算法可以从源序列复制使谓词返回true的元素，所以可以把它看作一个过滤器。
// 前两个参数定义源序列的输入迭代器，
// 第三个参数是指向目的序列的第一个位置的输出迭代器，第四个参数是一个谓词。
// 会返回一个输出迭代器，它指向最后一个被复制元素的下一个位置。
void func3() {
  std::copy_if(std::begin(v), std::end(v), std::inserter(us, std::begin(us)),
               [](const int& i) { return i <= 3; });
  std::copy(us.begin(), us.end(), std::ostream_iterator<int>{std::cout, " "});
  std::cout << std::endl;  // 3 2 10 9 8 1 6 5 4

  std::copy_if(std::begin(v), std::end(v),
               std::ostream_iterator<int>{std::cout, " "},
               [](const int& i) { return i > 5; });
  std::cout << std::endl;  // 6 7
}
void func4() {
  // 输入流迭代器可以作为copy_if()算法的源，也可以将它用在其他需要输入迭代器的算法上：
  std::unordered_set<std::string> names;
  std::cout << "Enter names of less than 5 letters. Enter Ctrl+Z on a separate "
               "line to end:\n";
  std::copy_if(std::istream_iterator<std::string>{std::cin},
               std::istream_iterator<std::string>{},
               std::inserter(names, std::begin(names)),
               [](const std::string& s) { return s.length() <= 4; });
  std::copy(std::begin(names), std::end(names),
            std::ostream_iterator<std::string>{std::cout, " "});
  std::cout << std::endl;
}

// 不要被copy_backward()算法的名称所误导，它不会逆转元素的顺序。
// 它只会像copy()那样复制元素，但是从最后一个元素开始直到第一个元素。
// copy_backward()会复制前两个迭代器参数指定的序列。
// 第三个参数是目的序列的结束迭代器，通过将源序列中的最后一个元素复制到目的序列的结束迭代器之前，
// 源序列会被复制到目的序列中。
// copy_backward()的3个参数都必须是可以自增或自减的双向迭代器，这意味着这个算法只能应用到序列容器的序列上。
// 从源序列的反向，将每一个元素依次复制到目的序列的前一个元素之前的位置。
// 在进行这个操作之前，目的序列中的元素必须存在，因此目的序列至少要有和源序列一样多的元素，但也可以有更多。
// copy_backward()算法会返回一个指向最后一个被复制元素的迭代器，在目的序列的新位置，它是一个开始迭代器。
// 相对于普通的从第一个元素开始复制的copy()算法，copy_backward()提供了哪些优势?
// 在序列重叠时，可以用copy()将元素复制到重叠的目的序列剩下的位置——也就是目的序列第一个元素之前的位置。
// 如果想尝试用copy()算法将元素复制到同一个序列的右边，这个操作不会成功，因为被复制的元素在复制之前会被重写。
// 如果想将它们复制到右边，可以使用copy_backward()，只要目的序列的结束迭代器在源序列的结束迭代器的右边。
// 在想将元素复制到右边时，copy()算法显然不能如我们所愿，因为一些元素在复制之前会被重写。
// 在这种情况下，copy_backward()可以做到我们想做的事。
// 相反在需要将元素复制到 序列的左边时，copy()可以做到，但copy_backward()做不到。
void func5() {
  // 为了能够在右边进行序列的反向复制操作，需要添加一些额外的元素，
  // 可以通过使用deque的成员函数resize()来增加deque容器的元素个数。
  // copy_backward()算法会将原有的元素复制到向右的两个位置，保持前两个元素不变
  std::deque<std::string> song{"jingle", "bells", "jingle",
                               "all",    "the",   "way"};
  song.resize(song.size() + 2);  // Add 2 elements
  std::copy_backward(std::begin(song), std::begin(song) + 6, std::end(song));
  std::copy(std::begin(song), std::end(song),
            std::ostream_iterator<std::string>{std::cout, " "});
  std::cout << std::endl;
  // jingle bells jingle bells jingle all the way
}

// reverse_copy()算法可以将源序列复制到目的序列中，目的序列中的元素是逆序的。
// 定义源序列的前两个迭代器参数必须是双向迭代器。
// 目的序列由第三个参数指定，它是目的序列的开始迭代器，也是一个输出迭代器。
// 如果序列是重叠的，函数的行为是未定义的。
// 这个算法会返回一个输出迭代器，它指向目的序列最后一个元素的下一个位置。
void func6() {
  // 这个程序会检查一条语句(也可以是很多条语句)是否是回文的。
  // 回文语句是指正着读或反着读都相同的句子，前提是忽略一些像空格或标点这样的细节。
  while (true) {
    std::string sentence;
    std::cout << "Enter a sentence or Ctrl+Z to end: ";
    std::getline(std::cin, sentence);
    if (std::cin.eof()) break;
    std::string only_letters;
    std::copy_if(std::begin(sentence), std::end(sentence),
                 std::back_inserter(only_letters),
                 [](char ch) { return std::isalpha(ch); });
    std::for_each(std::begin(only_letters), std::end(only_letters),
                  [](char& ch) { ch = toupper(ch); });
    std::string reversed;
    std::reverse_copy(std::begin(only_letters), std::end(only_letters),
                      std::back_inserter(reversed));
    std::cout << '"' << sentence << '"'
              << (only_letters == reversed ? " is" : " is not")
              << " a palindrome." << std::endl;
  }
}

void func7() {
  // reverse()算法可以在原地逆序它的两个双向迭代器参数所指定序列的元素。
  // 可以用下面的语句来代替上述程序中的reverse_copy():
  std::string reversed = "123456789";
  std::reverse(std::begin(reversed), std::end(reversed));
  std::copy_n(reversed.begin(), reversed.size(),
              std::ostream_iterator<char>(std::cout, " "));
  std::cout << std::endl;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      func1();
      func2();
      func3();
      func5();
      break;
    case 1:
      func4();
      break;
    case 2:
      func6();
      break;
    case 3:
      func7();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}