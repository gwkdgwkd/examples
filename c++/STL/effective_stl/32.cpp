#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <vector>

// 如果确实需要删除元素，则需要在remove这一类算法之后调用erase

// remove是STL中最令人感到疑惑的算法。
// remove也需要一对迭代器来指定所要进行操作的元素区间。
// 它并不接受容器作为参数，所以remove并不知道这些元素被存放在哪个容器中。
// 并且remove也不可能推断出是什么容器，因为无法从迭代器推知对应的容器类型。
// 唯一的办法是调用容器的成员函数，几乎总是erase的某种形式。
// list有几个可以删除元素的成员函数，但它们没有被命名为erase，
// 不过它们确实是成员函数，所以remove不可能从容器中删除元素。
// 这也说明一个现象：用remove从容器删除元素，而容器中的元素数目却不会因此而减少。
// remove不是真正意义上的删除，因为它做不到。

// remove移动了区间中的元素，其结果是，
// 不用被删除的元素移到了区间前部，保持原来的相对顺序。
// 它返回一个迭代器指向最后一个不用被删除的元素之后的元素，
// 相当于该区间新的逻辑结尾。
// 通常来说，当调用了remove后，
// 从区间中被删除的那些元素可能在也可能不在区间中。
// 如果真想删除元素，那就必须在remove之后使用erase。
// 把remove返回的迭代器作为区间形式erase的第一个实参是很常见的，
// 这是个习惯用法。
// remove和erase配合的如此紧密，
// 以至于它们被合并起来融入到了list的remove成员函数中。
// 这是STL中唯一一个名remove并且确实删除了元素的函数。
// list的remove也是不一致的地方，在关联容器中类似的函数被称为erase。
// 照理来说，list的remove也应该被称为erase。

// remove并不是唯一一个适用这种情形的算法，
// 其他还有两个属于remove类的算法：remove_if和unique。
// remove_if和remove的相似性是很显然的，但是unique也和remove行为相似。
// unique也需要在没有任何容器信息的情况下，
// 从容器中删除一些元素（相邻的、重复的值）。
// 所以如果真想从容器中删除元素的话，就必须在调用unique之后在调用erase。
// unique与list的结合也与remove的情形类似。
// 如同list::remove会真正删除元素一样，list::unique也会真正删除元素。

int main() {
  std::vector<int> v;
  v.reserve(10);
  for (int i = 1; i <= 10; ++i) {
    v.push_back(i);
  }
  std::cout << v.size() << std::endl;  // 10
  v[3] = v[5] = v[9] = 99;
  remove(v.begin(), v.end(), 99);
  std::cout << v.size() << std::endl;  // 10
  std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 2 3 5 7 8 9 8 9 99

  std::vector<int> v1;
  v1.reserve(10);
  for (int i = 1; i <= 10; ++i) {
    v1.push_back(i);
  }
  std::cout << v1.size() << std::endl;  // 10
  v1[3] = v1[5] = v1[9] = 99;
  v1.erase(remove(v1.begin(), v1.end(), 99), v1.end());
  std::cout << v1.size() << std::endl;  // 7
  std::copy(v1.begin(), v1.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 2 3 5 7 8 9

  std::list<int> li{1, 2, 99, 3, 4, 5, 6, 99, 88, 99};
  std::cout << li.size() << std::endl;  // 10
  li.remove(99);
  std::cout << li.size() << std::endl;  // 7
  std::copy(li.begin(), li.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 2 3 4 5 6 88

  return 0;
}