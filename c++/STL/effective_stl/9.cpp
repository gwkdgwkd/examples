#include <algorithm>
#include <deque>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

// 慎重选择删除元素的方法

// 删除容器中所有指定值的元素，完成这一任务的方式随容器类型而异，
// 没有对所有容器类型都适用的方式。
void func1() {
  // 连续内存的容器（vector、deque或string），
  // 那么最好的办法是使用erase-remove：
  std::vector<int> v{1, 2, 3, 4, 5, 3};
  v.erase(std::remove(v.begin(), v.end(), 3), v.end());
  std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 2 4 5

  std::deque<int> d{1, 2, 3, 4, 5, 3};
  d.erase(std::remove(d.begin(), d.end(), 3), d.end());
  std::copy(d.begin(), d.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 2 4 5

  std::string str = "123453";
  str.erase(std::remove(str.begin(), str.end(), '3'), str.end());
  std::copy(str.begin(), str.end(),
            std::ostream_iterator<char>(std::cout, " "));
  std::cout << std::endl;  // 1 2 4 5

  // 对list，erase和remove的办法一样适用：
  std::list<int> l{1, 2, 3, 4, 5, 3, 4};
  l.erase(std::remove(l.begin(), l.end(), 3), l.end());
  std::copy(l.begin(), l.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 2 4 5 4
  // 但list的成员函数remove更加有效：
  l.remove(4);
  std::copy(l.begin(), l.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 2 5

  // 对于标准关联容器（set、multiset、map或multimap）时，
  // 使用任何名为remove的操作都是完全错误的。
  // 这样的容器没有名为remove的成员函数，
  // 使用remove算法可能会覆盖容器的值，同时可能会破坏容器。
  // 对于关联容器，解决问题的正确方法是调用erase：cerase(1963);
  // 这样做不仅正确，而且高效，只需要对数时间的开销。
  // 关联容器的erase成员函数还有另外一个优点，它是基于等价而不是相等的。
  std::map<int, char> m{{1, 'a'}, {2, 'b'}, {3, 'c'}};
  m.erase(2);
  std::for_each(std::begin(m), std::end(m), [](const std::pair<int, char>& p) {
    std::cout << p.first << ":" << p.second << std::endl;
  });
  // 1:a
  // 3:c

  std::multimap<int, char> mm{{1, 'a'}, {2, 'b'}, {3, 'c'}, {2, 'd'}};
  mm.erase(2);
  std::for_each(std::begin(mm), std::end(mm),
                [](const std::pair<int, char>& p) {
                  std::cout << p.first << ":" << p.second << std::endl;
                });
  // 1:a
  // 3:c

  std::set<int> s{1, 2, 3};
  s.erase(2);
  std::copy(s.begin(), s.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 3

  std::multiset<int> ms{1, 2, 3, 2, 3};
  ms.erase(2);
  std::copy(ms.begin(), ms.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 3 3
}

bool badValue(int i) { return i % 2; }
bool badValue1(int i) { return i > 2; }
void func2() {
  // 对于序列容器（vector、string、deque和list），
  // 把每个remove的调用换成remove_if就可以了，
  // 这是删除使badValue返回true的对象的最好办法：
  std::vector<int> v{1, 2, 3, 4, 5, 3};
  v.erase(std::remove_if(v.begin(), v.end(), badValue), v.end());
  std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 2 4

  std::deque<int> d{1, 2, 3, 4, 5, 3};
  d.erase(std::remove_if(d.begin(), d.end(), badValue), d.end());
  std::copy(d.begin(), d.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 2 4

  std::string str = "123453";
  str.erase(std::remove_if(str.begin(), str.end(), badValue), str.end());
  std::copy(str.begin(), str.end(),
            std::ostream_iterator<char>(std::cout, " "));
  std::cout << std::endl;  // 2 4

  std::list<int> l{1, 2, 3, 4, 5, 3, 4};
  l.erase(std::remove_if(l.begin(), l.end(), badValue), l.end());
  std::copy(l.begin(), l.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 2 4 4
  l.remove_if(badValue1);
  std::copy(l.begin(), l.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 2
}

// 对于标准关联容器，有两种办法，一种易于编码，另一种则效率更高。
void func3() {
  // 简单但效率稍低的办法是，
  // 利用remove_copy_if把需要的值拷贝到一个新容器中，
  // 然后把原来容器的内容和新容器的内容互换，
  // 这种办法的缺点是需要拷贝所有不被删除的元素：
  std::map<int, char> m1{{1, 'a'}, {2, 'b'}, {3, 'c'}, {4, 'd'}};
  std::map<int, char> m2;
  std::remove_copy_if(
      m1.begin(), m1.end(), std::inserter(m2, m2.end()),
      [](const std::pair<int, char>& p) { return p.first % 2; });
  m1.swap(m2);
  std::for_each(std::begin(m1), std::end(m1),
                [](const std::pair<int, char>& p) {
                  std::cout << p.first << ":" << p.second << std::endl;
                });
  // 2:b
  // 4:d

  std::multimap<int, char> mm1{{1, 'a'}, {2, 'b'}, {3, 'c'}, {2, 'd'}};
  std::multimap<int, char> mm2;
  std::remove_copy_if(
      mm1.begin(), mm1.end(), std::inserter(mm2, mm2.end()),
      [](const std::pair<int, char>& p) { return p.first % 2; });
  mm1.swap(mm2);
  std::for_each(std::begin(mm1), std::end(mm1),
                [](const std::pair<int, char>& p) {
                  std::cout << p.first << ":" << p.second << std::endl;
                });
  // 2:b
  // 2:d

  std::set<int> s1{1, 2, 3, 4};
  std::set<int> s2;
  std::remove_copy_if(s1.begin(), s1.end(), std::inserter(s2, s2.end()),
                      badValue);
  s1.swap(s2);
  std::copy(s1.begin(), s1.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 2 4

  std::multiset<int> ms1{1, 2, 3, 2, 3};
  std::multiset<int> ms2;
  std::remove_copy_if(ms1.begin(), ms1.end(), std::inserter(ms2, ms2.end()),
                      badValue);
  ms1.swap(ms2);
  std::copy(ms1.begin(), ms1.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 2 2
}
void func4() {
  // 也可以直接从原始的容器中删除元素，从而降低代价。
  // 关联容器没有提供类似remove_if的成员函数，
  // 所以必须写一个循环来遍历元素，并在过程中删除元素。
  // 立刻想到的代码很少恰好是能工作的代码。比如，下面是很多成员首先能想到的：
  // AssocContainer<int> c;
  // for (AssocContainer<int>::iterator i = c.begin(); i != c.end(); ++i) {
  //   if (badValue(*i)) {
  //     c.erase(i);  // 一旦c.erase(i)返回，i就成为无效值。
  //   }
  // }
  // 这会导致不确定的行为。
  // 当容器中的一个元素被删除时，指向该元素的所有迭代器都将变得无效。
  // 为了避免这个问题，要确保在调用erase之前，
  // 有一个迭代器指向容器中的下一个元素。
  // 最简单的办法是，调用时对i使用后缀递增：
  // for (AssocContainer<int>::iterator i = c.begin(); i != c.end();) {
  //   if (badValue(*i)) {
  //     c.erase(i++);
  //   } else {
  //     ++i;
  //   }
  // }
  std::map<int, char> m{{1, 'a'}, {2, 'b'}, {3, 'c'}, {4, 'd'}};
  for (auto it = m.begin(); it != m.end();) {
    if (badValue(it->first)) {
      m.erase(it++);
    } else {
      ++it;
    }
  }
  std::for_each(std::begin(m), std::end(m), [](const std::pair<int, char>& p) {
    std::cout << p.first << ":" << p.second << std::endl;
  });
  // 2:b
  // 4:d

  std::multimap<int, char> mm{{1, 'a'}, {2, 'b'}, {3, 'c'}, {2, 'd'}};
  for (auto it = mm.begin(); it != mm.end();) {
    if (badValue(it->first)) {
      mm.erase(it++);
    } else {
      ++it;
    }
  }
  std::for_each(std::begin(mm), std::end(mm),
                [](const std::pair<int, char>& p) {
                  std::cout << p.first << ":" << p.second << std::endl;
                });
  // 2:b
  // 2:d

  std::set<int> s{1, 2, 3, 4};
  for (auto it = s.begin(); it != s.end();) {
    if (badValue(*it)) {
      s.erase(it++);
    } else {
      ++it;
    }
  }
  std::copy(s.begin(), s.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 2 4

  std::multiset<int> ms{1, 2, 3, 2, 3};
  for (auto it = ms.begin(); it != ms.end();) {
    if (badValue(*it)) {
      ms.erase(it++);
    } else {
      ++it;
    }
  }
  std::copy(ms.begin(), ms.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 2 2
}

void func5() {
  // 不仅要删除，还要在每次元素被删除时，都向一个日志文件中写一条信息。
  // 对于关联容器，这非常简单：
  std::multiset<int> ms{1, 2, 3, 2, 3};
  for (auto it = ms.begin(); it != ms.end();) {
    if (badValue(*it)) {
      std::cout << "remove " << *it << std::endl;
      // 对于标准关联容器，erase的返回类型是void，
      // 对于这类容器，得使用将传给erase的迭代器进行后缀递增的技术：
      ms.erase(it++);
    } else {
      ++it;
    }
  }
  std::copy(ms.begin(), ms.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;
  // remove 1
  // remove 3
  // remove 3
  // 2 2

  // 但对于vector、string和deque，
  // 不能再使用erase-remove惯用方法了，因为无法向日志中写信息。
  // 而且也不能使用关联容器的方法，因为会导致不确定的行为。
  // 这类容器调用erase不仅会使指向被删除元素的迭代器失效，
  // 也会使被删除元素之后的所有迭代器都无效。
  // 对于vector、string和deque，
  // 必须采取不同的策略，尤其要利用erase的返回值。
  // 一旦erase完成，它是指向紧随被删除元素的下一个元素的有效迭代器。
  // 可以这样写：
  // for (SeqContainer<int>::iterator i = c.begin(); i != c.end();) {
  //   if (badValue(*i)) {
  //     logFile << "Erasing " << *i << '\n';
  //     i = c.erase(i);  // 把erase的返回值赋给i，使i的值保持有效
  //   } else {
  //     ++i;
  //   }
  // }
  std::vector<int> v{1, 2, 3, 4, 5, 3};
  for (auto it = v.begin(); it != v.end();) {
    if (badValue(*it)) {
      std::cout << "remove " << *it << std::endl;
      it = v.erase(it);
    } else {
      ++it;
    }
  }
  std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;
  // remove 1
  // remove 3
  // remove 5
  // remove 3
  // 2 4

  // 对于list，遍历和删除时，
  // 可以把list当作vector/string/deque来对对待，
  // 也可以把它当作关联容器来对待。
  // 两种方式对list都适用，
  // 一般的惯例是对list采取和vector、string和deque相同的方式。
  std::list<int> l{1, 2, 3, 4, 5, 3, 6, 0};
  for (auto it = l.begin(); it != l.end();) {
    if (badValue(*it)) {
      std::cout << "remove " << *it << std::endl;
      it = l.erase(it);
    } else {
      ++it;
    }
  }
  std::copy(l.begin(), l.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;
  // remove 1
  // remove 3
  // remove 5
  // remove 3
  // 2 4 6 0
  for (auto it = l.begin(); it != l.end();) {
    if (badValue1(*it)) {
      std::cout << "remove " << *it << std::endl;
      l.erase(it++);
    } else {
      ++it;
    }
  }
  std::copy(l.begin(), l.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;
  // remove 4
  // remove 6
  // 2 0
}

// 总结：
// 一、要删除容器中有特定值的所有对象：
// 1.如果容器是vector、string或deque，
//   则使用erase-remove习惯用法。
// 2.如果容器是list，则使用list::remove。
// 3.如果容器是一个标准关联容器，则使用它的erase成员函数。
// 二、要删除容器中满足特定判别式（条件）的所有对象：
// 1.如果容器是vector、string或deque，
//   则使用erase-remove_if习惯用法。
// 2.如果容器是list，则使用list::remove_if。
// 3.如果容器是一个标准关联容器，
//   则使用remove_copy_if和swap，或者写一个循环来遍历容器中的元素，
//   记住当把迭代器传给erase时，进行后缀递增。
// 三、要在循环内部做某些（除了删除对象之外的）操作：
// 1.如果容器是一个标准序列容器，则写一个循环来遍历容器中的元素，
//   记住每次调用erase时，要用它的返回值更新迭代器。
// 2.如果容器是一个标准关联容器，则写一个循环来遍历容器中的元素，
//   记住当把迭代器传给erase时，要对迭代器做后缀递增。

int main() {
  func1();
  func2();
  func3();
  func4();
  func5();
  return 0;
}