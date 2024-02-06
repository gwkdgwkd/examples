#include <iostream>

#include "concrete_aggregate.h"
#include "concrete_iterator.h"

// Iterator模式用来解决对一个聚合对象的遍历问题，
// 将对聚合的遍历封装到一个类中进行，这样就避免了暴露这个聚合对象的内部表示的可能。

// Iterator模式实际上为了更好地保护Aggregate的状态，
// 可以尽量减小Aggregate的public接口，
// 而通过将Iterator对象声明位Aggregate的友元来给予Iterator一些特权，
// 获得访问Aggregate私有数据和方法的机会。

// Iterator模式的应用很常见，在开发中就经常会用到STL中预定义好的Iterator，
// 来对STL类Vector、Set等进行遍历。

int main(int argc, char* argv[]) {
  Aggregate* ag = new ConcreteAggregate();
  Iterator* it = new ConcreteIterator(ag);
  for (; !(it->IsDone()); it->Next()) {
    std::cout << it->CurrentItem() << " ";
  }
  std::cout << std::endl;  // 0 1 2

  return 0;
}