#include <iostream>
#include <iterator>
#include <vector>

using namespace std;

// C++ STL标准库为了方便用户更轻松地操作容器，每个容器的模板类都提供有丰富且实用的方法。在这些方法中，有些是以const_iterator类型迭代器作为参数，也就意味着在
// 使用此类方法时，需要为其传入一个const_iterator类型的迭代器。
// vector容器模板类中提供有insert()方法，该方法的语法格式如下：
// iterator insert (const_iterator position, const value_type& val);
// 是不是给insert()方法传递其它类型迭代器就不行呢？当然不是，对于给const_iterator类型的迭代器传值，还可以使用iterator类型迭代器，但不能使用const_reverse_iterator
// 和reverse_iterator类型迭代器，这是为什么呢？
// 当我们将某一类型的迭代器传递给insert()方法中const_iterator类型的position形参时，即便类型不匹配，编译器也不会立即报错，而是先尝试将其类型转换成const_iterator类型，
// 如果转换成功，则程序仍可以正常执行；反之如果转换失败，编译器才会报错。通常将编译器自行尝试进行类型转换的整个过程称为隐式转换（或者自动类型转换）。
// 对于C++ STL标准库中的这4种基础迭代器来说，C++编译器的隐式转换仅支持以下2种情况：
//  将iterator类型的迭代器隐式转换为const_iterator类型的迭代器；
//  将reverse_iterator类型的迭代器隐式转换为const_reverse_iterator类型的迭代器。
// 以上2种隐式转换是单向的，即编译器只支持从iterator转换为const_iterator，从reverse_iterator转换为const_reverse_iterator，但不支持逆向转换。
// 既然隐式转换无法做到，还有其他方式可以实现从const_iterator到 iterator、从const_reverse_iterator到reverse_iterator的转换吗？
// 可能会想到使用强制类型转换（const_cast）的方式。但可以明确的是，强制类型转换并不适用于迭代器，因为const_cast的功能仅是去掉某个类型的const修饰符，但const_iterator
// 和iterator是完全不同的2个类，同样const_reverse_iterator和reverse_iterator也是完全不同的2个类，它们仅仅是类名有const的差别，但并不是const T和T的关系。
// 推荐一种实现方式，就是使用advance()和distance()这2个函数，其语法格式如下：
// advance(iter, distance<cont<T>::const_iterator>(iter,citer));   // 将const_iterator转换为iterator
// advance(iter, distance<cont<T>::const_reverse_iterator>(iter,citer));  // 将const_reverse_iterator转换为reverse_iterator
// 其中，citer为指向某个容器（比如cont）任意位置的const_iterator（或者const_reverse_iterator）类型迭代器，而iter通常初始为指向cont容器中第一个元素的iterator（或
// 者reverse_iterator）类型迭代器。通过套用此格式，最终iter会变成一个指向和citer一样的iterator（或者reverse_iterator）类型迭代器。
// 注意，在使用distance()函数时，必须额外指明2个参数为const 迭代器类型，否则会因为传入的iter和citer类型不一致导致distance()函数编译出错。
// 该实现方式的本质是，先创建一个迭代器citer，并将其初始化为指向容器中第一个元素的位置。在此基础上，通过计算和目标迭代器iter的距离（调用distance()），将其移动至和iter同
// 一个位置（调用advance()），由此就可以间接得到一个指向同一位置的iter迭代器。
// 此方法的实现效率仍取决于目标容器的迭代器类型，如果是随机访问迭代器，则该方法的执行效率为O(1)；反之，则执行效率为O(n)。

int main() {
  vector<int> value{1, 2, 3, 4, 5};
  // 想调用此格式的insert()方法，就需要为其传入一个const_iterator类型的迭代器。
  vector<int>::const_iterator citer = value.cbegin();
  value.insert(citer, 10);
  for (auto iter = value.begin(); iter != value.end(); ++iter) {
    cout << *iter << " ";
  }
  cout << endl;  // 10 1 2 3 4 5

  vector<int> value1{1, 2, 3, 4, 5};
  vector<int>::const_iterator citer1 = --value.cend();
  vector<int>::iterator iter1 = value.begin();
  advance(iter1, distance<vector<int>::const_iterator>(iter1, citer1));
  cout << "*citer = " << *citer1 << endl;  // *citer = 5
  cout << "*iter = " << *iter1 << endl;    // *iter = 5
  // 通过使用advance()和distance()函数的组合格式，最终可以得到一个和citer指向相同但类型为iterator的迭代器。

  return 0;
}