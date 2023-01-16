#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <vector>

// swap_ranges
// swap
// iter_swap

// 可以用swap_ranges()算法来交换两个序列。
// 这个算法需要3个正向迭代器作为参数。
// 前两个参数分别是第一个序列的开始和结束迭代器，
// 第三个参数是第二个序列的开始迭代器。
// 显然，这两个序列的长度必须相同。
// 这个算法会返回一个迭代器，
// 它指向第二个序列的最后一个被交换元素的下一个位置。

// 定义在utility头文件中的swap()算法的重载函数的模板原型为：
// template<typename T1, typename T2>
// void swap(std::pair<T1,T2> left, std::pair<T1,T2> right);
// 这段代码会对pair<T1,T2>对象进行交换，
// 在前面的代码段中也可以用swap_ranges()来交换元素。
// 除了pair对象的重载之外，
// utility文件头中也有可以交换任何类型的容器对象的模板的重载。
// 也就是说，可以交换两个list<T>容器或者两个set<T>容器，
// 但不能是一个list<T>和vector<T>，也不能是一个list<T1>和一个list<T2>。
// 另一个swap()模板的重载可以交换两个相同类型的数组。
// 也有其他几个swap()的重载，它们可以用来交换其他类型的对象，
// 包含元组和智能指针类型。
// iter_swap()算法有一些不同，它会交换两个正向迭代器所指向的元素。

using Name = std::pair<std::string, std::string>;
int main() {
  std::vector<Name> people{Name{"Al", "Bedo"}, Name{"Ann", "Ounce"},
                           Name{"Jo", "King"}};
  std::list<Name> folks{Name{"Stan", "Down"}, Name{"Dan", "Druff"},
                        Name{"Bea", "Gone"}};
  std::swap_ranges(std::begin(people), std::begin(people) + 2,
                   ++std::begin(folks));

  // 并没有为了将pair对象写入流而重载operator<<()函数，
  // 因此copy()无法用输出流迭代器来列出容器的内容。
  // 为了生成输出，使用for_each()算法将lambda表达式运用到容器的每个元素上：
  std::for_each(std::begin(people), std::end(people), [](const Name& name) {
    std::cout << '"' << name.first << " " << name.second << "\" ";
  });
  std::cout << std::endl;  // "Dan Druff" "Bea Gone" "Jo King"
  std::for_each(std::begin(folks), std::end(folks), [](const Name& name) {
    std::cout << '"' << name.first << " " << name.second << "\" ";
  });
  std::cout << std::endl;  // "Stan Down" "Al Bedo" "Ann Ounce"

  return 0;
}