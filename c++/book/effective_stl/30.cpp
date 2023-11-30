#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <vector>

// 确保目标足够大

// STL最受欢迎的要属容器了，是STL最重要的成就之一，STL算法也有此殊荣。
// STL中有8个容器，却包含超过100个算法。

// 无论何时，如果使用的算法需要指定一个目标区间，
// 那么必须确保目标区间足够大，或者确保它会随着算法的运行而增大。
// 要在算法执行过程中增大目标区间，请使用插入性迭代器。
// 比如back_inserter、front_inserter和inserter返回的迭代器。

int transmogrify(int x) { return x * 2; }

std::vector<int> values{1, 2, 3, 4};
std::vector<int> results;

template <typename T>
void print(const T& v) {
  std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << "[" << v.size() << "]" << std::endl;
}
namespace n1 {
void func() {
  // transform调用是错误的，因为导致了对无效对象的赋值操作：
  transform(values.begin(), values.end(), results.end(), transmogrify);

  // 段错误
}
}  // namespace n1

namespace n2 {
void func() {
  results.reserve(results.size() + values.size());
  transform(values.begin(), values.end(), results.end(), transmogrify);
  // 上面的语句还是错误的，在results尾部未初始化的内存中进行赋值操作。
  // reserve只改变了容量不改变大小，即使凑巧能够完成所期望的赋值操作，
  // results也不会指定transform在尚未使用的内存中创建了新的对象。
  // results在transform调用后并不会改变，并且end仍然指向transform被调用前的位置。
  // 使用插入型迭代器back_inserter、front_inserter和inserter可以解决这个问题。

  print(results);  // [0]
}
}  // namespace n2

namespace n3 {
void func() {
  results.reserve(results.size() + values.size());
  transform(values.begin(), values.end(), back_inserter(results), transmogrify);
  // back_inserter可适用于所有提供了push_back方法的容器，
  // 所有标准序列容器：vector、string、deque和list。
  // 在头部插入调用front_inserter，在内部利用了push_front，
  // 所以front_inserter仅适用与提供了push_front的容器，deque和list。

  print(results);  // 2 4 6 8 [4]
}
}  // namespace n3

namespace n4 {
void func() {
  std::list<int> results;
  transform(values.begin(), values.end(), front_inserter(results),
            transmogrify);
  print(results);  // 8 6 4 2 [4]
  // 在results中的顺序将会与在values中的顺序相反。
  // vector没有提供push_front，无法使用front_inserter。

  results.clear();
  // 如果需要插入到前端，并顺序与原来一致，那只要以相反的顺序遍历就可以了：
  transform(values.rbegin(), values.rend(), front_inserter(results),
            transmogrify);
  print(results);  // 2 4 6 8 [4]
}
}  // namespace n4

namespace n5 {
void func() {
  // inserter将用于把算法的结果插入到容器中的特定位置：
  transform(values.begin(), values.end(),
            inserter(results, results.begin() + results.size() / 2),
            transmogrify);

  // 无论使用back_inserter、front_inserter还是inserter，
  // 算法的结果都会被逐个地插入到目标区间中。

  print(results);  // 2 4 6 8 [4]
}
}  // namespace n5

namespace n6 {
void func1() {
  results.assign({7, 8, 9});
  print(results);  // 7 8 9 [3]

  // 希望transform覆盖results，需要确保results中的元素至少和values中一样多：
  if (results.size() < values.size()) {
    results.resize(values.size());
  }
  transform(values.begin(), values.end(), results.begin(), transmogrify);
  print(results);  // 2 4 6 8 [4]
}

void func2() {
  results.assign({7, 8, 9});
  print(results);  // 7 8 9 [3]

  // 也可以先清空，然后安装通常的方式使用一个插入型迭代器：
  results.clear();
  results.reserve(values.size());
  transform(values.begin(), values.end(), back_inserter(results), transmogrify);
  print(results);  // 2 4 6 8 [4]
}

void func() {
  func1();
  func2();
}
}  // namespace n6

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
    case 3:
      n4::func();
      break;
    case 4:
      n5::func();
      break;
    case 5:
      n6::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
  return 0;
}