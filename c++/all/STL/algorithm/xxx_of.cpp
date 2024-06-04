#include <algorithm>
#include <iostream>
#include <vector>

// all_of
// any_of
// none_of
// count
// count_if

std::vector<int> ages{22, 19, 46, 75, 54, 19, 27, 66, 61, 33, 22, 19};

namespace n1 {
// algorithm中定义了3种算法，用来检查元素什么时候使谓词返回true。
// 这些算法的前两个参数是定义谓词应用范围的输入迭代器，第三个参数指定了谓词。
// 检查元素是否能让谓词返回true似乎很简单，但它却是十分有用的。
// 检查元素属性的三种算法是：
// all_of()算法返回true，前提是序列中的所有元素都可以使谓词返回true。
// any_of()算法返回true，前提是序列中的任意一个元素都可以使谓词返回true。
// none_of()算法返回true，前提是序列中没有元素可以使谓词返回true。
void func() {
  int min_age = 18;
  std::cout << "There are "
            << (std::none_of(std::begin(ages), std::end(ages),
                             [min_age](int age) { return age < min_age; })
                    ? "no"
                    : "some")
            << " people under " << min_age << std::endl;
  // There are no people under 18

  int min_age1{27};
  std::cout << "There are "
            << (std::none_of(std::begin(ages), std::end(ages),
                             [min_age1](int age) { return age < min_age1; })
                    ? "no"
                    : "some")
            << " people under " << min_age1 << std::endl;
  // There are some people under 27

  std::cout << "There are "
            << (std::any_of(std::begin(ages), std::end(ages),
                            [min_age](int age) { return age < min_age; })
                    ? "some"
                    : "no")
            << " people under " << min_age << std::endl;
  // There are no people under 18

  int good_age{100};
  std::cout << (std::all_of(std::begin(ages), std::end(ages),
                            [good_age](int age) { return age < good_age; })
                    ? "None"
                    : "Some")
            << " of the people are centenarians." << std::endl;
  // None of the people are centenarians.
}
}  // namespace n1

namespace n2 {
// count和count_if可以告诉，在前两个参数指定的范围内，
// 有多少满足指定的第三个参数条件的元素。
// count()会返回等同于第三个参数的元素的个数。
// count_if()会返回可以使作为第三个参数的谓词返回true的元素个数。
void func() {
  int the_age{19};
  std::cout << "There are "
            << std::count(std::begin(ages), std::end(ages), the_age)
            << " people aged " << the_age << std::endl;
  // There are 3 people aged 19

  int max_age{60};
  std::cout << "There are "
            << std::count_if(std::begin(ages), std::end(ages),
                             [max_age](int age) { return age > max_age; })
            << " people aged over " << max_age << std::endl;
  // There are 3 people aged over 60
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