#include <iostream>
#include <iterator>
#include <vector>

// STL为了让用户更轻松地操作容器，每个容器的模板类都提供有丰富且实用的方法。
// 在这些方法中，有些是以const_iterator类型迭代器作为参数，
// 也就意味着在使用此类方法时，需要为其传入const_iterator类型的迭代器。
// vector容器模板类中提供有insert()方法，该方法的语法格式如下：
// iterator insert(const_iterator position, const value_type& val);
// 是不是给insert()方法传递其它类型迭代器就不行呢？
// 当然不是，对于给const_iterator类型的迭代器，还可以使用iterator类型迭代器，
// 但不能用const_reverse_iterator和reverse_iterator，这是为什么呢？
// 当将迭代器传递给insert()的const_iterator类型的position参数时，
// 即便类型不匹配编译器也不会立即报错，而是先尝试转换成const_iterator类型，
// 如果转换成功，则程序仍可以正常执行，反之如果转换失败，编译器才会报错。
// 通常将编译器自行尝试进行类型转换的整个过程称为隐式转换（或者自动类型转换）。

// 对于C++ STL标准库中的这4种基础迭代器来说，隐式转换仅支持以下2种情况：
// 1.将iterator类型的迭代器隐式转换为const_iterator类型的迭代器；
// 2.将reverse_iterator类型隐式转换为const_reverse_iterator类型的迭代器。
// 以上2种隐式转换是单向的，即编译器只支持从iterator转换为const_iterator，
// 从reverse_iterator转换为const_reverse_iterator，但不支持逆向转换。
// 既然隐式转换无法做到，还有其他方式可以实现从const_iterator到iterator、
// 从const_reverse_iterator到reverse_iterator的转换吗？
// 可能会想到使用强制类型转换（const_cast）的方式。
// 但可以明确的是，强制类型转换并不适用于迭代器，
// 因为const_cast的功能仅是去掉某个类型的const修饰符，
// 但const_iterator和iterator是完全不同的2个类，
// 同样const_reverse_iterator和reverse_iterator也是完全不同的2个类，
// 它们仅仅是类名有const的差别，但并不是const T和T的关系。

// 推荐使用advance()和distance()这2个函数，其语法格式如下：
// advance(iter, distance<cont<T>::const_iterator>(iter,citer));
// advance(iter, distance<cont<T>::const_reverse_iterator>(iter,citer));
// 其中，citer为指向容器任意位置的const_iterator或const_reverse_iterator，
// 而iter通常初始化为容器中元素的iterator（或reverse_iterator）类型迭代器。
// 通过套用此格式，最终iter会变成一个指向和citer一样的，
// iterator（或者reverse_iterator）类型迭代器。
// 注意，在使用distance()函数时，必须额外指明2个参数为const迭代器类型，
// 否则会因为传入的iter和citer类型不一致导致distance()函数编译出错。
// 该实现方式的本质是，先创建迭代器citer，并将其初始化为指向容器中第一个元素的位置。
// 在此基础上，通过计算和目标迭代器iter的距离（调用distance()），
// 将其移动至和iter同一个位置（调用advance()），
// 由此就可以间接得到一个指向同一位置的iter迭代器。
// 此方法的实现效率仍取决于目标容器的迭代器类型，如果是随机访问迭代器，
// 则该方法的执行效率为O(1)，反之，则执行效率为O(n)。

int main() {
  std::vector<int> v{1, 2, 3, 4, 5};
  std::vector<int>::const_iterator cit = v.cbegin();
  // 想调用此格式的insert()方法，就需要为其传入const_iterator类型的迭代器：
  v.insert(cit, 10);
  // 隐式转换，从iterator到cosnt iterator的转换：
  v.insert(v.begin(), 11);
  for (auto it = v.begin(); it != v.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;  // 11 10 1 2 3 4 5

  cit = --v.cend();
  std::vector<int>::iterator it = v.begin();
  std::cout << "*cit = " << *cit << std::endl;  // *cit = 5
  std::cout << "*it = " << *it << std::endl;    // *it = 11

  // 通过使用advance()和distance()函数的组合格式，
  // 得到一个和citer指向相同但类型为iterator的迭代器：
  std::advance(it, std::distance<std::vector<int>::const_iterator>(it, cit));
  std::cout << "*cit = " << *cit << std::endl;  // *cit = 5
  std::cout << "*it = " << *it << std::endl;    // *it = 5

  return 0;
}