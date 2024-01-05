#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

// 条款13：优先使用const_iterator而不是iterator

// const_iterator等价于指向常量的指针pointer-to-const，它们都指向不能被修改的值。
// 能加上const就加上，即需要迭代器时只要没必要修改迭代器指向的值，就应当用const_iterator。

namespace n1 {
// 上面的说法对C++11和C++98都正确，但在C++98中，标准库对const_iterator的支持不是很完整。
// 首先不容易创建它们，其次就算你有了它，它的使用也是受限的。

// 实现功能，在std::vector<int>中查找第一次出现1983的位置，然后插入1998，
// 如果vector中没有1983，那么就在vector尾部插入。

namespace test1 {
// 在C++98中使用iterator可以很容易做到：
void func() {
  std::vector<int> values;
  std::vector<int>::iterator it = std::find(values.begin(), values.end(), 1983);
  values.insert(it, 1998);
}
}  // namespace test1

namespace test2 {
// iterator真的不是一个好的选择，因为这段代码不修改iterator指向的内容，
// 用const_iterator重写这段代码是很平常的，但是在C++98中就不是了：
typedef std::vector<int>::iterator IterT;  // typedef不是强制的
typedef std::vector<int>::const_iterator ConstIterT;
void func() {
  std::vector<int> values;
  ConstIterT ci = std::find(static_cast<ConstIterT>(values.begin()),
                            static_cast<ConstIterT>(values.end()), 1983);
  // values.insert(static_cast<IterT>(ci), 1998);

  // 之所以std::find的调用会出现类型转换是因为在C++98中values是non-const容器，
  // 没办法简简单单的从non-const容器中获取const_iterator。
  // 严格来说类型转换不是必须的，因为用其他方法获取const_iterator也是可以的，
  // 比如你可以把values绑定到reference-to-const变量上，然后再用这个变量代替values，
  // 但不管怎么说，从non-const容器中获取const_iterator的做法都有点别扭。

  // 当费劲地获得了const_iterator，事情可能会变得更糟，因为C++98中，
  // 插入操作（以及删除操作）的位置只能由iterator指定，const_iterator是不被接受的。
  // 这也是上面的代码中，将小心地从std::find搞出来的const_iterator转换为iterator的原因，
  // 因为向insert传入const_iterator不能通过编译。

  // 上面的代码无法编译，因为没有一个可移植的从const_iterator到iterator的方法，
  // 即使使用static_cast也不行，甚至传说中的牛刀reinterpret_cast也杀不了这条鸡。
  // 它不是C++98的限制，也不是C++11的限制，只是const_iterator就是不能转换为iterator，
  // 不管看起来对它们施以转换是有多么合理。

  // const_iterator在C++98中会有很多问题，不如iterator有用。
  // 最终，开发者们不再相信能加const就加它的教条，而是只在实用的地方加它。
}
}  // namespace test2

void func() {
  test1::func();
  test2::func();
}
}  // namespace n1

namespace n2 {
// 所有的这些都在C++11中改变了，现在const_iterator既容易获取又容易使用。
// 容器的成员函数cbegin和cend产出const_iterator，甚至对于non-const容器也可用，
// 那些之前使用iterator指示位置（如insert和erase）的STL成员函数也可以使用const_iterator了。
// 使用C++11 const_iterator重写C++98使用iterator的代码也稀松平常：
void func() {
  std::vector<int> values;
  auto it = std::find(values.cbegin(), values.cend(), 1983);
  values.insert(it, 1998);
}
}  // namespace n2

namespace n3 {
// 泛化findAndInsert：
#if __cplusplus > 201103L
// C++14工作良好：
template <typename C, typename V>
void findAndInsert(C& container, const V& targetVal, const V& insertVal) {
  auto it = std::find(std::cbegin(container), std::cend(container), targetVal);
  container.insert(it, insertVal);
}
#else
// 由于标准化的疏漏，C++11只添加了非成员函数begin和end，
// 但是没有添加cbegin，cend，rbegin，rend，crbegin，crend，C++14修订了这个疏漏。

// C++11可以简单实现非成员函数cbegin和cend：
template <class C>
auto cbegin(const C& container) -> decltype(std::begin(container)) {
  // 非成员函数cbegin没有调用成员函数cbegin，这个模板接受任何代表类似容器的数据结构的实参类型C，
  // 并且通过reference-to-const形参container访问这个实参。
  // 如果C是一个普通的容器类型，如std::vector<int>，container将会引用一个const版本的容器，
  // 如const vector<int>&，对const容器调用非成员函数begin返回const_iterator，
  // 这个迭代器也是模板要返回的，用这种方法实现的好处是就算容器只提供begin成员函数，
  // 不提供cbegin成员函数也没问题，那么现在可以将这个非成员函数cbegin施于只直接支持begin的容器。
  // 如果C是原生数组，这个模板也能工作，这时，container成为一个const数组的引用，
  // C++11为数组提供特化版本的非成员函数begin，它返回指向数组第一个元素的指针。
  // 一个const数组的元素也是const，所以对于const数组，非成员函数begin返回指向const的指针。
  // 在数组的上下文中，所谓指向const的指针，也就是const_iterator了。
  return std::begin(container);
}
template <class C>
auto cend(const C& container) -> decltype(std::end(container)) {
  return std::begin(container);
}

template <typename C, typename V>
void findAndInsert(C& container, const V& targetVal, const V& insertVal) {
  auto it = std::find(cbegin(container), cend(container), targetVal);
  container.insert(it, insertVal);
}
#endif

void func() {
  std::vector<int> values;
  findAndInsert(values, 1983, 1998);
}
}  // namespace n3

// 本条款的中心是鼓励只要能用就使用const_iterator。
// C++98就有的思想，但只是一般有用，到了C++11它就是极其有用了，C++14在其基础上做了些修补工作。

// 要记住的东西：
// 1.优先考虑const_iterator而非iterator；
// 2.在最大泛型代码中，优先考虑非成员函数的begin，end，rbegin等，而非同名成员函数。

int main(int argc, char* argv[]) {
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