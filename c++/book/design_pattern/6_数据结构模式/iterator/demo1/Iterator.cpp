template <typename T>
class Iterator {
 public:
  virtual void first() = 0;
  virtual void next() = 0;
  virtual bool isDone() const = 0;
  virtual T& current() = 0;
};

template <typename T>
class MyCollection {
 public:
  Iterator<T> GetIterator() {
    // ...
  }
};

template <typename T>
class CollectionIterator : public Iterator<T> {
  MyCollection<T> mc;

 public:
  CollectionIterator(const MyCollection<T>& c) : mc(c) {}

  void first() override {}
  void next() override {}
  bool isDone() const override {}
  T& current() override {}
};

// Iterator 迭代器

// 动机（Motivation）：
// 1.在软件构建过程中，集合对象内部结构常常变化各异，但对于这些集合对象，
//   希望不暴露其内部结构的同时，可以让外部客户代码透明地访问其中包含的元素，
//   同时这种透明遍历也为同一种算法在多种集合对象上进行操作提供了可能；
// 2.使用面向对象技术将这种遍历机制抽象为迭代器对象为应对变化中的集合对象提供了优雅的方式。

// 定义：
// 提供一种方法顺序访问一个聚合对象的各个元素，而又不暴露（稳定）该对象的内部表示。

// 要点总结：
// 1.迭代对象，访问一个聚合对象的内容而无需暴露它的内部表示；
// 2.迭代多态，为遍历不同的集合结构提供一个统一的接口，
//   从而支持同样的算法在不同的集合结构上进行操作；
// 3.迭代器的健壮性考虑，遍历的同时更改迭代器所在的集合结构，会导致问题；
// 4.面向对象的实现方式有性能损耗，现在STL使用模板（静态多态）实现，效率高，
//   也就是说这个模式有些过时了。

void MyAlgorithm() {
  MyCollection<int> mc;

  Iterator<int> iter = mc.GetIterator();

  for (iter.first(); !iter.isDone(); iter.next()) {
    cout << iter.current() << endl;
  }
}
