#include <algorithm>
#include <deque>
#include <iostream>
#include <iterator>
#include <set>
#include <vector>

// remove
// remove_copy
// remove_if
// remove_copy_if

// 如果不知道具体的场景，即元素保存在什么样的容器中，是不能从序列中移除元素的。
// 因此，移除元素的算法也无法做到这一点，它们只会重写被选择的元素或者忽略复制的元素。
// 移除操作不会改变被移除元素的序列的元素个数。

namespace n1 {
// remove()可以从它的前两个正向迭代器参数指定的序列中移除和第三个参数相等的对象。
// 基本上每个元素都是通过用它后面的元素覆盖它来实现移除的。
// 它会返回一个指向新的最后一个元素之后的位置的迭代器。

void func() {
  // remove()算法会通过左移其他元素来覆盖它们，通过这种方式就可以消除杂乱分布的0。
  // remove()返回的迭代器指向通过这个操作得到的新序列的尾部，
  // 所以可以用它作为被删除序列的开始迭代器来调用samples的成员函数erase()：
  std::deque<double> samples{1.5, 2.6, 0.0, 3.1, 0.0, 0.0, 4.1, 0.0, 6.7, 0.0};
  samples.erase(std::remove(std::begin(samples), std::end(samples), 0.0),
                std::end(samples));
  std::copy(std::begin(samples), std::end(samples),
            std::ostream_iterator<double>{std::cout, " "});
  std::cout << std::endl;  // 1.5 2.6 3.1 4.1 6.7
}
}  // namespace n1

namespace n2 {
// remove_copy()可以将前两个正向迭代器指定的元素复制到第三个参数指定的目的序列中，
// 并忽略和第4个参数相等的元素，它返回指向最后被复制到目的序列的元素的后一个位置的迭代器，
// 序列不能是重叠的。
void func() {
  // 如果想保留原始序列，并生成一个移除选定元素之后的副本，可以使用remove_copy()：
  std::deque<double> samples{1.5, 2.6, 0.0, 3.1, 0.0, 0.0, 4.1, 0.0, 6.7, 0.0};
  std::vector<double> edited_samples;
  std::remove_copy(std::begin(samples), std::end(samples),
                   std::back_inserter(edited_samples), 0.0);

  std::copy(std::begin(samples), std::end(samples),
            std::ostream_iterator<double>{std::cout, " "});
  std::cout << std::endl;  // 1.5 2.6 0 3.1 0 0 4.1 0 6.7 0

  std::copy(std::begin(edited_samples), std::end(edited_samples),
            std::ostream_iterator<double>{std::cout, " "});
  std::cout << std::endl;  // 1.5 2.6 3.1 4.1 6.7
}
}  // namespace n2

using Name = std::pair<std::string, std::string>;
namespace n3 {
// remove_if()可以从前两个正向迭代器指定的序列中移除使第三个参数的谓词返回true的元素。
void func() {
  // remove_if()提供了更强大的能力，它能够从序列中移除和给定值匹配的元素。
  // 谓词会决定一个元素是否被移除，它接受序列中的一个元素为参数，并返回一个布尔值：
  std::set<Name> blacklist{Name{"Al", "Bedo"}, Name{"Ann", "Ounce"},
                           Name{"Jo", "King"}};
  std::deque<Name> candidates{Name{"Stan", "Down"}, Name{"Al", "Bedo"},
                              Name{"Dan", "Druff"}, Name{"Di", "Gress"},
                              Name{"Ann", "Ounce"}, Name{"Bea", "Gone"}};
  candidates.erase(std::remove_if(std::begin(candidates), std::end(candidates),
                                  [&blacklist](const Name& name) {
                                    return blacklist.count(name);
                                  }),
                   std::end(candidates));
  std::for_each(std::begin(candidates), std::end(candidates),
                [](const Name& name) {
                  std::cout << '"' << name.first << " " << name.second << "\" ";
                });
  std::cout << std::endl;  // "Stan Down" "Dan Druff" "Di Gress" "Bea Gone"
}
}  // namespace n3

namespace n4 {
// remove_copy_if()可以将前两个正向迭代器参数指定的序列中，
// 能够使作为第4个参数的谓词返回true的元素，复制到第三个参数指定的目的序列中。
// 它返回一个指向最后一个被复制到目的序列的元素的后一个位置的迭代器，序列不能是重叠的。
// remove_copy_if()之于remove_copy()，就像remove_if()之于remove。
void func() {
  std::set<Name> blacklist{Name{"Al", "Bedo"}, Name{"Ann", "Ounce"},
                           Name{"Jo", "King"}};
  std::deque<Name> candidates{Name{"Stan", "Down"}, Name{"Al", "Bedo"},
                              Name{"Dan", "Druff"}, Name{"Di", "Gress"},
                              Name{"Ann", "Ounce"}, Name{"Bea", "Gone"}};
  std::deque<Name> validated;
  std::remove_copy_if(
      std::begin(candidates), std::end(candidates),
      std::back_inserter(validated),
      [&blacklist](const Name& name) { return blacklist.count(name); });

  std::for_each(std::begin(validated), std::end(validated),
                [](const Name& name) {
                  std::cout << '"' << name.first << " " << name.second << "\" ";
                });
  std::cout << std::endl;  // "Stan Down" "Dan Druff" "Di Gress" "Bea Gone"
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