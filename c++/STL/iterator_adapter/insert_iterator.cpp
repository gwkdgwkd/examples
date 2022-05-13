#include <forward_list>
#include <iostream>
#include <iterator>
#include <list>
#include <queue>

// 插入迭代器适配器（insert_iterator），简称插入迭代器或者插入器，其功能就是向指定容器中插入元素。
// 值得一提的是，根据插入位置的不同，C++ STL标准库提供了3种插入迭代器
// 1.back_insert_iterator在指定容器的尾部插入新元素，
//   但前提必须是提供有push_back()成员方法的容器（包括vector、deque和list）。
// 2.front_insert_iterator在指定容器的头部插入新元素，
//   但前提必须是提供有push_front()成员方法的容器（包括list、deque和forward_list）。
// 3.insert_iterator在容器的指定位置之前插入新元素，前提是该容器必须提供有insert()成员方法。
// back_insert_iterator，front_insert_iterator，
// insert_iterator迭代器定义在<iterator>头文件，并位于std命名空间中。

// back_insert_iterator迭代器可用于在指定容器的末尾处添加新元素。
// 由于此类型迭代器的底层实现需要调用指定容器的push_back()成员方法，这就意味着，
// 此类型迭代器并不适用于STL标准库中所有的容器，它只适用于包含push_back()成员方法的容器。
// C++ STL标准库中，提供有push_back()成员方法的容器包括vector、deque和list。
// 和反向迭代器不同，back_insert_iterator插入迭代器的定义方式仅有一种，其语法格式如下：
// std::back_insert_iterator<Container> back_it(container);
// 其中，Container用于指定插入的目标容器的类型；container用于指定具体的目标容器。
// back_insert_iterator迭代器模板类中还对赋值运算符（=）进行了重载，
// 借助此运算符，我们可以直接将新元素插入到目标容器的尾部。
// C++ STL标准库为了方便用户创建back_insert_iterator类型的插入迭代器，提供了back_inserter()函数，其语法格式如下：
// template <class Container> back_insert_iterator<Container> back_inserter(Container& x);
// 显然在使用该函数时，只需要为其传递一个具体的容器（vector、deque或者list）做参数，
// 此函数即可返回一个back_insert_iterator类型的插入迭代器。

// 和back_insert_iterator正好相反，front_insert_iterator迭代器的功能是向目标容器的头部插入新元素。
// 由于此类型迭代器的底层实现需要借助目标容器的push_front()成员方法，这意味着，
// 只有包含push_front()成员方法的容器才能使用该类型迭代器。
// C++ STL标准库中，提供有push_front()成员方法的容器，仅有deque、list和forward_list。
// 定义front_insert_iterator迭代器的方式和 back_insert_iterator完全相同，
// 并且C++ STL标准库也提供了front_inserter()函数来快速创建该类型迭代器。

// 当需要向容器的任意位置插入元素时，就可以使用insert_iterator类型的迭代器。
// 该类型迭代器的底层实现，需要调用目标容器的insert()成员方法。
// 幸运的是，STL标准库中所有容器都提供有insert()成员方法，
// 因此insert_iterator是唯一可用于关联式容器的插入迭代器。
// 定义insert_iterator类型迭代器的语法格式如下：
// std::insert_iterator<Container> insert_it (container,it);
// 其中，Container表示目标容器的类型，参数container表示目标容器，
// 而it是一个基础迭代器，表示新元素的插入位置。
// 和前2种插入迭代器相比，insert_iterator迭代器除了定义时需要多传入一个参数，它们的用法完全相同。
// 除此之外，C++ STL标准库中还提供有inserter()函数，可以快速创建insert_iterator类型迭代器。
// 如果insert_iterator迭代器的目标容器为关联式容器，由于该类型容器内部会自行对存储的元素进行排序，
// 因此我们指定的插入位置只起到一个提示的作用，即帮助关联式容器从指定位置开始，搜索正确的插入位置。
// 但是，如果提示位置不正确，会使的插入操作的效率更加糟糕。

// 3种插入迭代器，虽然它们都可以借助重载的赋值运算符，实现向目标容器的指定位置插入新元素，
// 但在实际应用中，它们通常和copy()函数连用，即作为copy()函数的第3个参数。

int main() {
  std::vector<int> v;
  std::back_insert_iterator<std::vector<int> > back_it1(v);
  std::back_insert_iterator<std::vector<int> > back_it2 = std::back_inserter(v);
  back_it1 = 5;
  back_it2 = 4;
  back_it1 = 3;
  back_it2 = 6;
  for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    std::cout << *it << ' ';
  }
  std::cout << std::endl;  // 5 4 3 6

  std::forward_list<int> fl;
  std::front_insert_iterator<std::forward_list<int> > front_it1(fl);
  std::front_insert_iterator<std::forward_list<int> > front_it2 =
      std::front_inserter(fl);
  front_it1 = 5;
  front_it2 = 4;
  front_it1 = 3;
  front_it2 = 6;
  for (std::forward_list<int>::iterator it = fl.begin(); it != fl.end(); ++it) {
    std::cout << *it << ' ';
  }
  std::cout << std::endl;  // 6 3 4 5

  std::list<int> l{6, 5};
  std::list<int>::iterator it = ++l.begin();
  std::insert_iterator<std::list<int> > insert_it1(l, it);
  std::insert_iterator<std::list<int> > insert_it2 = std::inserter(l, it);
  insert_it1 = 1;
  insert_it2 = 2;
  insert_it2 = 3;
  insert_it1 = 4;
  for (std::list<int>::iterator it = l.begin(); it != l.end(); ++it) {
    std::cout << *it << ' ';
  }
  std::cout << std::endl;  // 6 1 2 3 4 5

  return 0;
}