#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

// 正确理解有reverse_iterator的base()成员函数所产生的iterator的用法

std::vector<int> v{1, 2, 3, 4, 5};
std::vector<int>::reverse_iterator ri = find(v.rbegin(), v.rend(), 3);
std::vector<int>::iterator i(ri.base());

namespace n1 {
void func() {
  std::cout << "reverse_iterator: " << *ri << std::endl;  // 3
  std::cout << "iterator: " << *i << std::endl;           // 4
  // reverse_iterator与base产生的iterator之间存在偏移，
  // 这段偏移勾画出了rbegin()和rend()与对应的begin()和end()之间的偏移。
}
}  // namespace n1

namespace n2 {
void func() {
  v.insert(i, 80);
  std::copy(v.cbegin(), v.cend(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 2 3 80 4 5
  // 如果要在一个reverse_iterator指定的位置上插入新元素，则只需在base()位置处插入即可。
  // 对于插入操作ri和ri.base()是等价的，ri.base()是真正与ri对应的iterator。

  ri = std::find(v.rbegin(), v.rend(), 3);
  i = ri.base();
  std::cout << "reverse_iterator: " << *ri << std::endl;  // 3
  std::cout << "iterator: " << *i << std::endl;           // 80

  v.erase(i);
  std::copy(v.cbegin(), v.cend(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 2 3 4 5
}
}  // namespace n2

namespace n3 {
void func() {
  v.insert(i, 80);
  std::copy(v.cbegin(), v.cend(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;                                 // 1 2 3 80 4 5

  ri = std::find(v.rbegin(), v.rend(), 3);
  i = ri.base();
  std::cout << "reverse_iterator: " << *ri << std::endl;  // 3
  std::cout << "iterator: " << *i << std::endl;           // 80

  v.erase(--i);
  std::copy(v.cbegin(), v.cend(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 2 80 4 5
  // 如果要在reverse_iterator指定的位置上删除元素，则需要在base()前面的位置上删除。
  // 对于删除操作ri和ri.base()是不等价的，ri.base()不是与ri对应的iterator。
  // 对于vector和string，这段代码或许也能工作，但对于许多实现，它无法通过编译。
  // 因为iterator是以内置指针来实现的，C和C++都规定了从函数返回的指针不应该被修改，
  // 所以，如果在STL平台上string和vector的iterator是指针的话，
  // 那么--ri.base()这样的表达式就无法通过编译。
  // 因此，出于通用性和可移植性的考虑，想要在reverse_iterator指定的位置上删除元素，
  // 应该避免直接修改base()的返回值，可以先递增reverse_iterator，再调用base()即可：
  v.insert(i, 3);
  ri = find(v.rbegin(), v.rend(), 3);
  i = ri.base();
  std::copy(v.cbegin(), v.cend(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 2 3 80 4 5

  v.erase((++ri).base());
  std::copy(v.cbegin(), v.cend(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 2 80 4 5

  // 由此可见通过base()可得到与reverse_iterator对应的iterator的说法并不准确。
  // 对于插入操作，这种对应关系确实存在，但对于删除操作，情况却并非如此简单。
}
}  // namespace n3

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
  return 0;
}