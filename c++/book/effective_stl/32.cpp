#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <vector>

// 如果确实需要删除元素，则需要在remove这一类算法之后调用erase

template <typename T>
void print(T t) {
  std::copy(t.begin(), t.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << "[" << t.size() << "]" << std::endl;
}

namespace n1 {
// remove是STL中最令人感到疑惑的算法，需要一对迭代器来指定所要操作的元素区间。
// 它并不接受容器作为参数，所以remove并不知道这些元素被存放在哪个容器中。
// 并且remove也不可能推断出是什么容器，因为无法从迭代器推知对应的容器类型。

// 要删除元素唯一的办法是调用容器的成员函数，几乎总是erase的某种形式。
// list有几个可以删除元素的成员函数，但它们没有被命名为erase，
// 不过它们确实是成员函数，所以remove不可能从容器中删除元素。
// 这也说明一个现象：用remove从容器删除元素，而容器中的元素数目却不会因此而减少。
// remove不是真正意义上的删除，因为它做不到。

// remove移动了区间中的元素，不用被删除的元素移到了区间前部，保持原来的相对顺序。
// 它返回一个迭代器指向最后一个不用被删除的元素之后的元素，相当于该区间新的逻辑结尾。
// 通常来说，当调用了remove后，从区间中被删除的那些元素可能在也可能不在区间中。
// 如果真想删除元素，那就必须在remove之后使用erase。
// 把remove返回的迭代器作为区间形式erase的第一个实参是很常见的，这是个习惯用法。
// remove和erase配合的如此紧密，以至于它们被合并起来融入到了list的remove成员函数中。
// 这是STL中唯一一个名remove并且确实删除了元素的函数。
// list的remove也是不一致的地方，在关联容器中类似的函数被称为erase。
// 照理来说，list的remove也应该被称为erase。

std::vector<int> v{1, 99, 5, 4, 7, 6, 99, 9, 8, 99};

namespace test1 {
void func1() {
  print(v);  // 1 99 5 4 7 6 99 9 8 99 [10]
  auto it = remove(v.begin(), v.end(), 99);
  // remove返回的迭代器指向新序列最后一个元素后面一个位置：
  std::cout << "v[" << it - v.begin() << "]=" << *it << std::endl;  // v[7]=9
  print(v);  // 1 5 4 7 6 9 8 9 8 99 [10]
}

void func2() {
  print(v);  // 1 99 5 4 7 6 99 9 8 99 [10]
  v.erase(std::remove(v.begin(), v.end(), 99), v.end());
  print(v);  // 1 5 4 7 6 9 8 [7]
}
}  // namespace test1

namespace test2 {
void func() {
  std::list<int> l(v.begin(), v.end());
  print(l);
  l.remove(99);
  print(l);
}
}  // namespace test2
}  // namespace n1

namespace n2 {
// remove并不是唯一一个适用这种情形的算法，还有两个算法：remove_if和unique。
// remove_if和remove的相似性是很显然的，但是unique也和remove行为相似。
// unique也需要在没有任何容器信息的情况下，从容器中删除相邻的、重复的元素。
// 所以如果真想从容器中删除元素的话，就必须在调用unique之后在调用erase。
// unique与list的结合也与remove类似，如同list::remove会真正删除元素一样，
// list::unique也会真正删除元素。

std::vector<int> v{1, 99, 5, 4, 7, 6, 99, 9, 8, 99};

namespace test1 {
void func1() {
  print(v);  // 1 99 5 4 7 6 99 9 8 99 [10]

  auto it = remove_if(v.begin(), v.end(), [](int i) { return i < 5; });
  // remove_if返回的迭代器指向新序列最后一个元素后面一个位置：
  std::cout << "v[" << it - v.begin() << "]=" << *it << std::endl;  // v[8]=8
  print(v);  // 99 5 7 6 99 9 8 99 8 99 [10]

  std::sort(v.begin(), v.end());
  auto it1 = unique(v.begin(), it);
  // unique返回的迭代器指向新序列最后一个元素的位置：
  std::cout << "v[" << it1 - v.begin() << "]=" << *it1 << std::endl;  // v[6]=99
  print(v);  // 5 6 7 8 9 99 99 99 99 99 [10]
}

void func2() {
  print(v);  // 1 99 5 4 7 6 99 9 8 99 [10]

  v.erase(std::remove_if(v.begin(), v.end(), [](int i) { return i < 5; }),
          v.end());
  print(v);  // 99 5 7 6 99 9 8 99 [8]

  std::sort(v.begin(), v.end());
  v.erase(std::unique(v.begin(), v.end()), v.end());
  print(v);  // 5 6 7 8 9 99 [6]
}
}  // namespace test1

namespace test2 {
void func() {
  std::list<int> l(v.begin(), v.end());
  print(l);  // 1 99 5 4 7 6 99 9 8 99 [10]

  l.remove_if([](int i) { return i < 5; });
  print(l);  // 99 5 7 6 99 9 8 99 [8]

  l.sort();
  l.unique();
  print(l);  // 5 6 7 8 9 99 [6]
}
}  // namespace test2
}  // namespace n2

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 5]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::test1::func1();
      break;
    case 1:
      n1::test1::func2();
      break;
    case 2:
      n1::test2::func();
      break;
    case 3:
      n2::test1::func1();
      break;
    case 4:
      n2::test1::func2();
      break;
    case 5:
      n2::test2::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
  return 0;
}
