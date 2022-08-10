#include <iostream>
#include <iterator>
#include <vector>

// 反向迭代器适配器（reverse_iterator），可简称为反向迭代器或逆向迭代器，
// 常用来对容器进行反向遍历，即从容器中存储的最后一个元素开始，
// 一直遍历到第一个元素。
// 反向迭代器底层可以选用双向迭代器或者随机访问迭代器作为其基础迭代器。
// 不仅如此，通过对++（递增）和--（递减）运算符进行重载，使得：
// 1.当反向迭代器执行++运算时，底层的基础迭代器实则在执行--操作，
//   意味着反向迭代器在反向遍历容器；
// 2.当反向迭代器执行--运算时，底层的基础迭代器实则在执行++操作，
//   意味着反向迭代器在正向遍历容器。

// 反向迭代器的模板类定义如下：
// template <class Iterator> class reverse_iterator;
// Iterator模板参数指的是模板类中所用的基础迭代器的类型，
// 只能选择双向迭代器或者随机访问迭代器。
// 这意味着，如果想使用反向迭代器实现逆序遍历容器，
// 则该容器的迭代器类型必须是双向迭代器或者随机访问迭代器。

// 反向迭代器的模板类定义在<iterator>头文件，并位于std命名空间中。
// operator* 	 以引用的形式返回当前迭代器指向的元素。
// operator+ 	 返回一个反向迭代器，其指向距离当前指向的元素之后n个位置的元素。
//             此操作要求基础迭代器为随机访问迭代器。
// operator++  重载前置++和后置++运算符。
// operator+=  当前反向迭代器前进n个位置，此操作要求基础迭代器为随机访问迭代器。
// operator- 	 返回一个反向迭代器，其指向距离当前指向的元素之前n个位置的元素。
//             此操作要求基础迭代器为随机访问迭代器。
// operator--  重载前置--和后置--运算符。
// operator-=  当前反向迭代器后退n个位置，此操作要求基础迭代器为随机访问迭代器。
// operator->  返回一个指针，其指向当前迭代器指向的元素。
// operator[n] 访问和当前反向迭代器相距n个位置处的元素。

int main() {
  // reverse_iterator模板类中共提供了3种创建反向迭代器的方法：
  // 1.调用该类的默认构造方法，即可创建了一个不指向任何对象的反向迭代器：
  std::reverse_iterator<std::vector<int>::iterator> it1;

  // 2.在创建反向迭代器的时候，
  //   可以直接将一个基础迭代器作为参数传递给新建的反向迭代器：
  std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8};
  std::reverse_iterator<std::vector<int>::iterator> it2(v.end());
  // 反向迭代器是通过操纵内部的基础迭代器实现逆向遍历的，
  // 但是反向迭代器的指向和底层基础迭代器的指向并不相同。
  // it2其内部的基础迭代器指向的是v容器中元素5之后的位置，
  // 但是it指向的却是元素5。
  // 反向迭代器的指向和其底层基础迭代器的指向具有这样的关系，
  // 即反向迭代器的指向总是距离基础迭代器偏左1个位置；
  // 反之，基础迭代器的指向总是距离反向迭代器偏右1个位置处。
  // 其中，begin和end表示基础迭代器，
  // r(begin)和r(end)分别表示有begin和end获得的反向迭代器。
  std::cout << *it2 << std::endl;                  // 8
  std::cout << v.end() - it2.base() << std::endl;  // 0

  // 3.reverse_iterator模板类还提供了一个复制（拷贝）构造函数，
  //   可以实现直接将一个反向迭代器复制给新建的反向迭代器：
  std::reverse_iterator<std::vector<int>::iterator> it3(v.rbegin());
  std::reverse_iterator<std::vector<int>::iterator> it4 = v.rbegin();

  std::reverse_iterator<std::vector<int>::iterator> it(v.rbegin());
  std::cout << *it << std::endl;           // 8
  std::cout << *(it + 3) << std::endl;     // 5
  std::cout << *(++it) << std::endl;       // 7
  std::cout << *(it.base()) << std::endl;  // 8
  std::cout << it[4] << std::endl;         // 3

  // reverse_iterator模板类还提供了base()成员方法，
  // 该方法可以返回当前反向迭代器底层所使用的基础迭代器。
  std::reverse_iterator<std::vector<int>::iterator> begin(v.begin());
  std::reverse_iterator<std::vector<int>::iterator> end(v.end());
  for (auto iter = begin.base(); iter != end.base(); ++iter) {
    std::cout << *iter << ' ';
  }
  std::cout << std::endl;  // 1 2 3 4 5 6 7 8

  return 0;
}