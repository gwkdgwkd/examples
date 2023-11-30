#include <algorithm>
#include <forward_list>
#include <iostream>
#include <iterator>
#include <list>
#include <queue>
#include <set>

// 插入迭代器适配器（insert_iterator），简称插入迭代器或者插入器，
// 其功能就是向指定容器中插入元素，值得一提的是，根据插入位置的不同，
// C++ STL标准库提供了3种插入迭代器：
// 1.back_insert_iterator在指定容器的尾部插入新元素，
//   但必须是提供push_back()成员方法的容器，包括vector、deque和list。
// 2.front_insert_iterator在指定容器的头部插入新元素，
//   但必须是提供push_front()成员方法的容器，包括list、deque和forward_list。
// 3.insert_iterator在容器的指定位置之前插入新元素，
//   前提是该容器必须提供有insert()成员方法。
// back_insert_iterator，front_insert_iterator，
// insert_iterator迭代器定义在<iterator>头文件，并位于std命名空间中。

// 3种插入迭代器，虽然它们都可以借助重载的赋值运算符，
// 实现向目标容器的指定位置插入新元素，但在实际应用中，
// 它们通常和copy()函数连用，即作为copy()函数的第3个参数。

template <typename T>
void print(T t) {
  for (typename T::iterator it = t.begin(); it != t.end(); ++it) {
    std::cout << *it << ' ';
  }
  std::cout << std::endl;
}

namespace n1 {
// back_insert_iterator迭代器可用于在指定容器的末尾处添加新元素。
// 和反向迭代器不同，back_insert_iterator插入迭代器的定义方式仅有一种：
// std::back_insert_iterator<Container> back_it(container);
// Container用于指定插入的目标容器的类型，container用于指定具体的目标容器。
// back_insert_iterator迭代器模板类中还对赋值运算符（=）进行了重载，
// 借助此运算符，可以直接将新元素插入到目标容器的尾部。
// C++ STL标准库为了方便用户创建back_insert_iterator类型的插入迭代器，
// 提供了back_inserter()函数，其语法格式如下：
// template <class Container>
// back_insert_iterator<Container> back_inserter(Container& x);
// 显然在使用该函数时，只需要为其传递一个具体的vector、deque或list做参数，
// 此函数即可返回一个back_insert_iterator类型的插入迭代器。

std::vector<int> v1;
std::deque<int> q1;
std::list<int> l1;
void func1() {
  std::back_insert_iterator<std::vector<int>> v_back_it1(v1);
  std::back_insert_iterator<std::vector<int>> v_back_it2 =
      std::back_inserter(v1);
  v_back_it1 = 1;
  v_back_it2 = 2;
  v_back_it1 = 3;
  v_back_it2 = 4;
  print(v1);  // 1 2 3 4

  std::back_insert_iterator<std::deque<int>> q_back_it(q1);
  q_back_it = 5;
  q_back_it = 6;
  q_back_it = 7;
  print(q1);  // 5 6 7

  std::back_insert_iterator<std::list<int>> l_back_it(l1);
  l_back_it = 8;
  l_back_it = 9;
  l_back_it = 10;
  print(l1);  // 8 9 10
}

void func2() {
  std::vector<int> v2;
  std::copy(v1.begin(), v1.end(),
            std::back_insert_iterator<std::vector<int>>(v2));
  print(v2);  // 1 2 3 4

  std::deque<int> q2;
  std::copy(q1.begin(), q1.end(),
            std::back_insert_iterator<std::deque<int>>(q2));
  print(q2);  // 5 6 7

  std::list<int> l2;
  std::copy(l1.begin(), l1.end(),
            std::back_insert_iterator<std::list<int>>(l2));
  print(l2);  // 8 9 10
}

void func() {
  func1();
  func2();
}
}  // namespace n1

namespace n2 {
// 和back_insert_iterator相反，front_insert_iterator向容器头部插入新元素。
// 定义front_insert_iterator迭代器的方式和back_insert_iterator完全相同，
// 并且C++ STL标准库也提供了front_inserter()函数来快速创建该类型迭代器。

std::deque<int> q1;
std::list<int> l1;
std::forward_list<int> f1;
void func1() {
  std::front_insert_iterator<std::deque<int>> q_front_it(q1);
  q_front_it = 3;
  q_front_it = 2;
  q_front_it = 1;
  print(q1);  // 1 2 3

  std::front_insert_iterator<std::list<int>> l_front_it(l1);
  l_front_it = 6;
  l_front_it = 5;
  l_front_it = 4;
  print(l1);  // 4 5 6

  std::front_insert_iterator<std::forward_list<int>> f_front_it(f1);
  f_front_it = 9;
  f_front_it = 8;
  f_front_it = 7;
  print(f1);  // 7 8 9
}

void func2() {
  std::deque<int> q2;
  std::copy(q1.rbegin(), q1.rend(),
            std::front_insert_iterator<std::deque<int>>(q2));
  print(q2);  // 1 2 3

  std::deque<int> q3;
  std::copy(q1.begin(), q1.end(),
            std::back_insert_iterator<std::deque<int>>(q3));
  print(q3);  // 1 2 3

  std::list<int> l2;
  std::copy(l1.rbegin(), l1.rend(),
            std::front_insert_iterator<std::list<int>>(l2));
  print(l2);  // 4 5 6

  std::list<int> l3;
  std::copy(l1.begin(), l1.end(),
            std::front_insert_iterator<std::list<int>>(l3));
  print(l3);  // 6 5 4

  std::forward_list<int> f2;  // 没有rbegin和rend
  std::copy(f1.begin(), f1.end(),
            std::front_insert_iterator<std::forward_list<int>>(f2));
  print(f2);  // 9 8 7
}

void func() {
  func1();
  func2();
}
}  // namespace n2

namespace n3 {
// 当需要向容器的任意位置插入元素时，就可以使用insert_iterator类型的迭代器。
// 该类型迭代器的底层实现，需要调用目标容器的insert()成员方法。
// 幸运的是，STL标准库中除forward_list外所有容器都提供有insert()成员方法，
// 因此insert_iterator是唯一可用于关联式容器的插入迭代器。
// 定义insert_iterator类型迭代器的语法格式如下：
// std::insert_iterator<Container> insert_it(container,it);
// 其中，Container表示目标容器的类型，参数container表示目标容器，
// 而it是一个基础迭代器，表示新元素的插入位置。
// 和前2种插入迭代器相比，insert_iterator除了多一个参数，它们的用法完全相同。
// STL还提供了inserter()函数，可以快速创建insert_iterator类型迭代器。
// 目标容器为关联式容器时，由于该类型容器内部会自行对存储的元素进行排序，
// 因此指定的插入位置只起到一个提示的作用，即帮助关联式容器从指定位置开始，
// 搜索正确的插入位置，但如果提示位置不正确，会使的插入操作的效率更加糟糕。

void func() {
  std::vector<int> v{1, 6};
  std::vector<int>::iterator it = ++v.begin();
  std::insert_iterator<std::vector<int>> v_insert_it(v, it);
  v_insert_it = 2;
  v_insert_it = 3;
  v_insert_it = 4;
  v_insert_it = 5;
  print(v);  // 1 2 3 4 5 6

  std::deque<int> q;
  std::copy(v.begin(), v.end(),
            std::insert_iterator<std::deque<int>>(q, q.begin()));
  print(q);  // 1 2 3 4 5 6

  q.clear();
  std::copy(v.begin(), v.end(),
            std::insert_iterator<std::deque<int>>(q, q.end()));
  print(q);  // 1 2 3 4 5 6

  std::list<int> l;
  std::copy(v.rbegin(), v.rend(),
            std::insert_iterator<std::list<int>>(l, l.begin()));
  print(l);  // 6 5 4 3 2 1

  l.clear();
  std::copy(v.rbegin(), v.rend(),
            std::insert_iterator<std::list<int>>(l, l.end()));
  print(l);  // 6 5 4 3 2 1

  std::set<int> m;
  std::copy(v.rbegin(), v.rend(),
            std::insert_iterator<std::set<int>>(m, m.begin()));
  print(m);  // 1 2 3 4 5 6
}
}  // namespace n3

int main(int argc, char *argv[]) {
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
