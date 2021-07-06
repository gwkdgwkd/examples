#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

using namespace std;

// 正确理解有reverse_iterator的base()成员函数所产生的iterator的用法

int main() {
  vector<int> v;
  v.reserve(5);
  for (int i = 1; i <= 5; ++i) {
    v.push_back(i);
  }
  vector<int>::reverse_iterator ri = find(v.rbegin(), v.rend(), 3);
  vector<int>::iterator i(ri.base());
  cout << "reverse_iterator: " << *ri << endl;  // 3
  cout << "iterator: " << *i << endl;           // 4
  // reverse_iterator与base产生的iterator之间存在偏移，这段偏移勾画出了rbegin()和rend()与对应的begin()和end()之间的偏移。

  v.insert(i, 80);
  copy(v.cbegin(), v.cend(), ostream_iterator<int>(cout, " "));
  cout << endl;  // 1 2 3 80 4 5
  // 如果要在一个reverse_iterator指定的位置上插入新元素，则只需在base()位置处插入即可。
  // 对于插入操作ri和ri.base()是等价的，ri.base()是真正与ri对应的iterator。

  ri = find(v.rbegin(), v.rend(), 3);
  i = ri.base();
  v.erase(i);
  copy(v.cbegin(), v.cend(), ostream_iterator<int>(cout, " "));
  cout << endl;  // 1 2 3 4 5

  v.insert(i, 80);
  ri = find(v.rbegin(), v.rend(), 3);
  i = ri.base();
  copy(v.cbegin(), v.cend(), ostream_iterator<int>(cout, " "));
  cout << endl;  // 1 2 3 80 4 5
  v.erase(--i);
  copy(v.cbegin(), v.cend(), ostream_iterator<int>(cout, " "));
  cout << endl;  // 1 2 80 4 5
  // 如果要在一个reverse_iterator指定的位置上删除一个元素，则需要在base()前面的位置上执行删除操作。
  // 对于删除操作ri和ri.base()是不等价的，ri.base()不是与ri对应的iterator。
  // 对于vector和string，这段代码或许也能工作，但对于许多实现，它无法通过编译。因为iterator是以内置指针的方式来实现的。
  // C和C++都规定了从函数返回的指针不应该被修改，所以，如果在STL平台上string和vector的iterator是指针的话，
  // 那么--ri.base()这样的表达式就无法通过编译。
  // 因此，出于通用性和可移植性的考虑，想要在一个reverse_iterator指定的位置上删除一个元素，应该避免直接修改base()的返回值。
  // 可以先递增reverse_iterator，然后在调用base()函数即可：
  v.insert(i, 3);
  ri = find(v.rbegin(), v.rend(), 3);
  i = ri.base();
  copy(v.cbegin(), v.cend(), ostream_iterator<int>(cout, " "));
  cout << endl;  // 1 2 3 80 4 5
  v.erase((++ri).base());
  copy(v.cbegin(), v.cend(), ostream_iterator<int>(cout, " "));
  cout << endl;  // 1 2 80 4 5

  // 由此可见，通过base()函数可以得到一个与reverse_iterator“相对应的”iterator的说法并不准确。
  // 对于插入操作，这种对应关系确实存在；
  // 但对于删除操作，情况却并非如此简单。

  return 0;
}