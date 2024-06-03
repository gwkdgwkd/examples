#include <math.h>

#include <atomic>
#include <iostream>
#include <mutex>
#include <vector>

// 条款16：让const成员函数线程安全

namespace n1 {
class Polynomial {  // 表示多项式的类
 public:
  using RootsType = std::vector<double>;

  // 计算多项式的根是很复杂的，因此如果不需要的话，就不做，如果必须做，
  // 肯定不想再做第二次，所以，如果必须计算它们，就缓存多项式的根：
  RootsType roots() const {
    if (!rootsAreValid) {  // 如果缓存不可用
      // 计算根
      rootsAreValid = true;  // 用rootVals存储它们
    }

    return rootVals;
  }
  // 从概念上讲，roots并不改变它所操作的Polynomial对象。
  // 但是作为缓存的一部分，它也许会改变rootVals和rootsAreValid的值。
  // 这就是mutable的经典使用样例，这也是为什么它是数据成员声明的一部分。

 private:
  mutable bool rootsAreValid{false};
  mutable RootsType rootVals{};
};

void func() {
  Polynomial p;
  auto rootsOfp = p.roots();        // 线程1
  auto valsGivingZero = p.roots();  // 线程2

  // 这些用户代码是非常合理的，roots是const成员函数，那就表示着它是一个读操作。
  // 在没有同步的情况下，让多个线程执行读操作是安全的，它最起码应该做到这点。
  // 在Polynomial却没有做到线程安全，因为在roots中，
  // 这些线程中的一个或两个可能尝试修改成员变量rootsAreValid和rootVals。
  // 这就意味着在没有同步的情况下，这些代码会有不同的线程读写相同的内存，
  // 这就是数据竞争（data race）的定义，这段代码的行为是未定义的。
}
}  // namespace n1

namespace n2 {
// n1的问题就是roots被声明为const，但不是线程安全的。
// const声明在C++11中与在C++98中一样正确，因此需要纠正的是线程安全的缺乏。
// 解决这个问题最普遍简单的方法就是，使用mutex（互斥量）：
class Polynomial {
 public:
  using RootsType = std::vector<double>;

  RootsType roots() const {
    std::lock_guard<std::mutex> g(m);

    if (!rootsAreValid) {
      rootsAreValid = true;
    }

    return rootsVals;
  }

 private:
  // m被声明为mutable，因为锁定和解锁都是non-const成员函数。
  // 而roots是const成员函数，调用m的non-const成员函数，需要m为mutable的。
  // 值得注意的是，因为std::mutex是只可移动类型，所以Polynomial不能复制只能移动。
  mutable std::mutex m;
  mutable bool rootsAreValid{false};
  mutable RootsType rootsVals{};
};
}  // namespace n2

namespace n3 {
// 在某些情况下，互斥量的副作用显会得过大。
// 例如，如果只是计算成员函数被调用了多少次，使用std::atomic通常会是一个开销更小的方法。
// 以下是如何使用std::atomic来统计调用次数：
class Point {
 public:
  double distanceFromOrigin() const noexcept {
    ++callCount;
    return std::sqrt((x * x) + (y * y));
  }

 private:
  // std::atomic是只可移动类型，所以在Point中存在callCount就意味着Point也是只可移动的：
  mutable std::atomic<unsigned> callCount{0};
  double x, y;
};

namespace test1 {
// 因为对std::atomic变量的操作通常比互斥量的获取和释放的消耗更小，
// 所以可能会过度倾向与依赖std::atomic，例如，在一个类中，
// 缓存一个开销昂贵的int，就会尝试使用一对std::atomic变量而不是互斥量。
class Widget {
 public:
  int magicValue() const {
    if (cacheValid) {
      return cachedValue;
    } else {
      auto val1 = [] { return 10; }();  // 复杂计算
      auto val2 = [] { return 20; }();  // 复杂计算
      cachedValue = val1 + val2;        // 第一步
      cacheValid = true;                // 第二步
      return cachedValue;
    }
    // 这是可行的，但难以避免有时出现重复计算的情况，考虑：
    // 1.一个线程调用Widget::magicValue，将cacheValid视为false，
    //   执行这两个昂贵的计算，并将它们的和分配给cachedValue。
    // 2.第二个线程调用Widget::magicValue，也将cacheValid视为false，
    //   因此执行刚才完成的第一个线程相同的计算。
    // 这种行为与使用缓存的目的背道而驰。
  }

 private:
  mutable std::atomic<bool> cacheValid{false};
  mutable std::atomic<int> cachedValue;
};
}  // namespace test1

namespace test2 {
class Widget {
 public:
  int magicValue() const {
    if (cacheValid) {
      return cachedValue;
    } else {
      auto val1 = [] { return 10; }();
      auto val2 = [] { return 20; }();
      // 将cachedValue和CacheValid的赋值顺序交换可以解test1的问题，但结果会更糟：
      cacheValid = true;
      return cachedValue = val1 + val2;
    }
    // 假设cacheValid是false，那么：
    // 1.一个线程调用Widget::magicValue，刚执行完将cacheValid设置true的语句；
    // 2.在这时，第二个线程调用Widget::magicValue，检查cacheValid，看到它是true，
    //   就返回cacheValue，即使第一个线程还没有给它赋值，因此返回的值是不正确的。
  }

 private:
  mutable std::atomic<bool> cacheValid{false};
  mutable std::atomic<int> cachedValue;
};
}  // namespace test2

namespace test3 {
class Widget {
 public:
  // 这里有一个坑，对于需要同步的是单个的变量或者内存位置，使用std::atomic就足够了。
  // 不过，一旦需要对两个以上的变量或内存位置作为一个单元来操作的话，就应该使用互斥量：
  int magicValue() const {
    std::lock_guard<std::mutex> guard(m);

    if (cacheValid) {
      return cachedValue;
    } else {
      auto val1 = [] { return 10; }();
      auto val2 = [] { return 20; }();
      cachedValue = val1 + val2;
      cacheValid = true;
      return cachedValue;
    }
  }

 private:
  mutable std::mutex m;
  mutable int cachedValue;         // 不再用atomic
  mutable bool cacheValid{false};  // 不再用atomic
};
}  // namespace test3
}  // namespace n3

// 现在，这个条款是基于，多个线程可以同时在一个对象上执行一个const成员函数这个假设的。
// 如果不是在这种情况下编写const成员函数，可以保证在对象上永远不会有多个线程执行该成员函数，
// 该函数的线程安全是无关紧要的，为独占单线程使用而设计的类的成员函数是否线程安全并不重要。
// 在这种情况下，可以避免因使用互斥量和std::atomics所消耗的资源，以及只能移动带来的副作用。
// 然而，这种线程无关的情况越来越少见，而且很可能会越来越少。
// 可以肯定的是，const成员函数应支持并发执行，这就是为什么应该确保const成员函数是线程安全的。

// 要记住的东西：
// 1.确保const成员函数线程安全，除非确定它们永远不会在并发上下文中使用；
// 2.使用std::atomic变量可能比互斥量提供更好的性能，但是它只适合操作单个变量或内存位置。

int main(int argc, char* argv[]) { return 0; }