#include <iostream>
#include <string>
#include <vector>

// 使用reserve来避免不必要的重新分配

// STL容器在放入数据时，只要没有超出max_size函数最大限制就可以自动增长。
// 对于vector和string，每次需要更多空间时，就调用与realloc类似的操作：
// 1.分配一块大小为当前容量的某个倍数的新内容，大多数实现中，
//   vector和string每次以2的倍数增长，即扩充时加倍；
// 2.把容器的所有元素从旧内存拷贝到新的内存中；
// 3.析构掉旧内存中的对象；
// 4.释放旧内存。
// 整个过程非常耗时，而且vector和string中所有的指针、迭代器和引用都变得无效。
// reserve成员函数能使你把重新分配的次数减少到最低限度，
// 从而避免重新分配和指针、迭代器、引用失效带来的开销。

// 只有vector和string提供了下面所有4个函数：
// 1.size()告诉该容器中有多少个元素，它不会告诉该容器为自己分配了多少内存；
// 2.capacity()容器已经分配的内存可以容纳多少个元素，是总数，
//   而不是还能容纳多少个，还能容纳多少个元素用capacity()-size()，
//   如果capacity()和size()相同，说明没有剩余空间了，插入将导致重新分配内存；
// 3.resize(Containter::size_type n)强迫容器改变到包含n个元素的状态，
//   如果n比当前的大小（size）要小，则容器尾部的元素将会被析构，
//   如果n比当前的大小要大，则通过默认构造函数创建的新元素将被添加到容器的末尾，
//   如果n比当前的容量要大，那么在添加元素之前，将先重新分配内存；
// 4.reserve(Containter::size_type n)
//   强迫容器把它的容量变为至少是n，前提是n不小于当前的大小，这通常会导致重新分配，
//   因为容量需要增加，如果n比当前容量小，则vector忽略该调用，什么也不做，
//   而string则可能把自己的容量减为size()和n中的最大值，但string的大小肯定不变，
//   使用reserve从string中除去多余的容量通常不如使用swap技巧。
// 避免重新分配的关键在于，尽早使用reserve，把容器的容量设为足够大的值，
// 最好是在容器刚被构造出来之后就使用reserve。

// 两种方式来避免不必要的重新分配：
// 1.若能确切知道或大致预计容器中最终会有多少个元素，则可使用reserve；
// 2.根据需要预留足够大的空间，然后把所有数据都加入以后，在去除多余的容量。

namespace n1 {
void func() {
  std::vector<int> v;
  // 对于大多数STL实现，该循环会导致2到10次重新分配：
  for (int i = 1; i <= 1000; ++i) v.push_back(i);

  std::vector<int> v1;
  v.reserve(1000);
  // 循环过程中，将不会再发生重新分配：
  for (int i = 1; i <= 1000; ++i) v1.push_back(i);

  // 大小和容量之间的关系使能够预知，什么时候插入会导致vector或string重新分配，
  // 进而有可能预知什么时候会导致迭代器、指针和引用失效：
  std::string s;
  if (s.size() < s.capacity()) {
    s.push_back('x');
  }
}
}  // namespace n1

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 0]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
  return 0;
}