#include <algorithm>
#include <iostream>
#include <list>
#include <string>

// 通过复合塑模出has-a或根据某物实现出

// 复合是类型之间的一种关系，当某种类型的对象内含其他类型的对象时，便是这种关系。
class Address {};
class PhoneNumber {};
// Person对象由string，Address，PhoneNumber构成：
class Person {
 private:
  std::string name;
  Address address;
  PhoneNumber voieNumber;
  PhoneNumber faxNumber;
};
// 程序员之间复合有许多同义词，包括分层，内含，聚合，内嵌，意味着：
// 1.has-a，有一个；
// 2.is-implemented-in-terms-of，根据某物实现出。
// is-a和has-a很好区分，但是is-a和is-implemented-in-terms-of区分起来比较麻烦。

// 实现一组class用来表示不重复对象组成的set，首先想到STL的set，
// 但是set招致每个元素用三个指针的额外开销，因为set通常以平衡查找树实现而成，
// 使它们在查找、插入、移除元素时保证拥有对数时间效率，速度比空间重要。
// 但如果你的程序空间比速度重要呢？
// 需要自己写template，实现set的方法很多，可以使用标准库的list template。
// template <typename T>
// class Set : public std::list<T> {};
// 使用public继承看起来对，实际是错误的，Set和list不是is-a的关系。
// 正确的做法是：
template <class T>
class Set {
 public:
  bool member(const T& item) const {
    return find(rep.begin(), rep.end(), item) != rep.end();
  }
  void insert(const T& item) {
    if (!member(item)) {
      rep.push_back(item);
    }
  }
  void remove(const T& item) {
    typename std::list<T>::iterator it = std::find(rep.begin(), rep.end(), item);
    if (it != rep.end()) {
      rep.erase(it);
    }
  }
  size_t size() const { return rep.size(); }

 private:
  std::list<T> rep;
};

// Set和list之间是is-implemented-in-terms-of关系。

// 请记住：
// 1.复合（composition）的意义和public继承完全不同；
// 2.在应用域，复合意味着has-a，有一个；
// 3.在实现域，复合意味着is-implemented-in-terms-of，根据某物实现出。

int main() { return 0; }