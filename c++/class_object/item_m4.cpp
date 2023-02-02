#include <iostream>

using namespace std;

// 避免无用的缺省构造函数

// 缺省构造函数（指没有参数的构造函数）在C++语言中是一种让你无中生有的方法。
// 构造函数能初始化对象，而缺省构造函数则可以不利用任何外部数据，
// 就在建立对象时初始化对象，有时这样的方法是不错的。
// 例如一些行为特性与数字相仿的对象被初始化为空值或不确定的值也是合理的，
// 还有比如链表、哈希表、图等等数据结构也可以被初始化为空容器。
// 但不是所有的对象都属于上述类型，对于很多对象来说，
// 不利用外部数据进行完全的初始化是不合理的。

// 无需任何数据即可建立对象的类可以包含缺省构造函数，
// 而需要数据来建立对象的类则不能包含缺省构造函数。
// 如果一个类没有缺省构造函数，就会存在一些使用上的限制。
class EquipmentPiece {
 public:
  EquipmentPiece(int IDNumber) {}
};
// 在三种情况下使用EquipmentPiece，就会遇到问题:
// 1.第一种情况是建立数组时；
//   一般来说，没有一种办法能在建立对象数组时给构造函数传递参数，
//   有三种方法能回避开这个限制。

// 2.对于类里没有定义缺省构造函数所造成的第二个问题是，
//   它们无法在许多基于模板（template-based）的容器类里使用。
//   因为实例化一个模板时，模板的类型参数应该提供一个缺省构造函数，
//   这是一个常见的要求。
//   这个要求总是来自于模板内部，被建立的模板参数类型数组里。
//   在多数情况下，通过仔细设计模板可以杜绝对缺省构造函数的需求，
//   例如标准的vector模板。
template <class T>
class Array {
 public:
  Array(int size);

 private:
  T* data;
};
template <class T>
Array<T>::Array(int size) {
  data = new T[size];  // 为每个数组元素依次调用T::T()
}

// 3.第三个问题是，在设计虚基类时所面临的：
//   要提供缺省构造函数还是不提供缺省构造函数的两难决策。
//   不提供缺省构造函数的虚基类，很难与其进行合作。
//   因为几乎所有的派生类在实例化时都必须给虚基类构造函数提供参数。
//   这就要求所有由没有缺省构造函数的虚基类继承下来的派生类，
//   都必须知道并理解提供给虚基类构造函数的参数的含义。
//   派生类的作者是不会企盼和喜欢这种规定的。

// 因为这些强加于没有缺省构造函数的类上的种种限制，
// 一些人认为所有的类都应该有缺省构造函数，
// 即使缺省构造函数没有足够的数据来完整初始化一个对象。
// 这个原则的拥护者会这样修改EquipmentPiece类：
class EquipmentPiece1 {
 public:
  EquipmentPiece1(int IDNumber = UNSPECIFIED);

 private:
  static const int UNSPECIFIED;
};
// 这样的修改使得其他成员函数变得复杂，
// 因为不再能确保EquipmentPiece对象进行了有意义的初始化。

// 提供无意义的缺省构造函数也会影响类的工作效率。
// 如果成员函数必须测试所有的部分是否都被正确地初始化，
// 那么这些函数的调用者就得为此付出更多的时间。
// 而且还得付出更多的代码，因为这使得可执行文件或库变得更大。

// 如果一个类的构造函数能够确保所有的部分被正确初始化，所有这些弊病都能够避免。
// 缺省构造函数一般不会提供这种保证，
// 所以在它们可能使类变得没有意义时，尽量去避免使用它们。
// 使用这种（没有缺省构造函数的）类的确有一些限制，
// 但是当你使用它时，它也给你提供了一种保证：
// 你能相信这个类被正确地建立和高效地实现。

int main() {
  // EquipmentPiece bestPieces[3];
  // EquipmentPiece *bestPieces =  new EquipmentPiece[3];

  // 对于使用非堆数组的一种解决方法是在数组定义时提供必要的参数：
  int ID1, ID2, ID3;
  EquipmentPiece bestPieces[] = {EquipmentPiece(ID1), EquipmentPiece(ID2),
                                 EquipmentPiece(ID3)};
  // 不过很遗憾，这种方法不能用在堆数组(heap arrays)的定义上。

  // 一个更通用的解决方法是利用指针数组来代替一个对象数组：
  typedef EquipmentPiece* PEP;
  PEP bestPieces1[10];             // 正确, 没有调用构造函数
  PEP* bestPieces2 = new PEP[10];  // 也正确
  // 在指针数组里的每一个指针被重新赋值，
  // 以指向一个不同的EquipmentPiece对象：
  for (int i = 0; i < 10; ++i) {
    bestPieces1[i] = new EquipmentPiece(i);
  }
  // 不过这种方法有两个缺点：
  // 1.你必须删除数组里每个指针所指向的对象。
  //   如果你忘了，就会发生内存泄漏。
  // 2.增加了内存分配量，
  //   因为正如你需要空间来容纳EquipmentPiece对象一样，
  //   你也需要空间来容纳指针。

  // 如果你为数组分配raw memory，你就可以避免浪费内存。
  // 使用placement new方法在内存中构造EquipmentPiece对象：
  void* rawMemory = operator new[](10 * sizeof(EquipmentPiece));
  EquipmentPiece* bestPieces3 = static_cast<EquipmentPiece*>(rawMemory);
  for (int i = 0; i < 10; ++i) {
    // 注意你仍旧得为每一个EquipmentPiece对象提供构造函数参数：
    new (&bestPieces[i]) EquipmentPiece(i);
  }
  // 这个技术和指针数组的主意一样，
  // 允许你在没有缺省构造函数的情况下建立一个对象数组。
  // 它没有绕过对构造函数参数的需求，实际上也做不到。
  // 如果能做到的话，就不能保证对象被正确初始化。
  // 使用placement new的缺点除了是大多数程序员对它不熟悉外，
  // 还有就是当你不想让它继续存在使用时，必须手动调用数组对象的析构函数，
  // 然后调用操作符delete[]来释放raw memory。
  for (int i = 9; i >= 0; --i) {
    bestPieces[i].~EquipmentPiece();
  }
  operator delete[](rawMemory);
  // 直接删除一个不是用new操作符来分配的内存指针，其结果没有被定义：
  // delete [] bestPieces;

  // Array<int> a;

  return 0;
}