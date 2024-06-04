#include <iostream>
#include <vector>

// 使用swap技巧除去多余的容量

// 为了避免占用不再需要的内存，需要把容器的容量从以前的最大值缩减到当前需要的数量。
// 这种对容量的缩减常被称为shrink to fit，但并不保证一定能去除多余的容量。
// STL实现者如果愿意的话，可以为vector和string保留多余的容量，有时确实希望这样做。
// 例如，可能需要一个最小的容量，或者把容量限制为2的乘幂数。
// shrink to fit并不意味着是容量尽量小，
// 意味着在容器当前大小确定的情况下，是容量在该实现下变为最小。

// 关于swap技巧，或者关于一般性的swap，不仅两个容器的内容被交换，
// 同时它们的迭代器、指针和引用也将被交换（string除外）。
// 在swap发生后，原先指向某容器中元素的迭代器、指针和引用仍然有效，
// 并指向同样的元素，但是这些元素已经在另一个容器中了。

namespace n1 {
void func() {
  std::vector<int> v;
  v.reserve(50);
  v.push_back(1);
  v.push_back(2);
  std::cout << "size:" << v.size() << std::endl;          // 2
  std::cout << "capacity:" << v.capacity() << std::endl;  // 50

  // vector<int>(v)创建了临时变量，是v的拷贝，由vector的拷贝构造函数完成。
  // 然而拷贝构造函数只为所拷贝的元素分配内存，所以临时变量没有多余的容量。
  // 然后把临时变量中的数据和v中的数据做swap操作，最后，v具有了被去除之后的容量，
  // 即临时变量的容量，而临时变量具有原先v臃肿的容量，
  // 在语句结尾，临时变量被析构，从而释放了先前为v所占据的内存。
  std::vector<int>(v).swap(v);
  std::cout << "size:" << v.size() << std::endl;          // 2
  std::cout << "capacity:" << v.capacity() << std::endl;  // 2

  // swap技巧还可以用来清除一个容器，并使其容量变为该实现下的最小值。
  std::vector<int>().swap(v);
  std::cout << "size:" << v.size() << std::endl;          // 0
  std::cout << "capacity:" << v.capacity() << std::endl;  // 0
}
}  // namespace n1

namespace n2 {
void func() {
  // 同样的技巧对string也适用：
  std::string s("hello");
  s.reserve(50);
  std::cout << s.size() << "," << s.capacity() << std::endl;  // 5,50

  std::string(s).swap(s);
  std::cout << s.size() << "," << s.capacity() << std::endl;  // 5,15

  std::string().swap(s);
  std::cout << s.size() << "," << s.capacity() << std::endl;  // 0,15
}
}  // namespace n2

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
  return 0;
}