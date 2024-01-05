#include <iostream>
#include <memory>
#include <unordered_map>

// 条款20：当std::shared_ptr可能悬空时使用std::weak_ptr

// 自相矛盾的是，如果有一个像shared_ptr的但是不参与资源所有权共享的指针是很方便的。
// 换句话说，是一个类似std::shared_ptr但不影响对象引用计数的指针。
// 这种类型的智能指针必须要解决一个shared_ptr不存在的问题：可能指向已经销毁的对象。
// 一个真正的智能指针应该跟踪所指对象，在悬空时知晓，悬空就是指针指向的对象不再存在。
// 这就是对std::weak_ptr最精确的描述。

// 可能想知道什么时候该用std::weak_ptr，也可能想知道关于std::weak_ptr的更多。
// 它什么都好除了不太智能，std::weak_ptr不能解引用，也不能测试是否为空值。
// 因为std::weak_ptr不是一个独立的智能指针，它是std::shared_ptr的增强。

struct Widget {};
namespace n1 {

void func1() {
  auto spw = std::make_shared<Widget>();

  // std::weak_ptr通常从std::shared_ptr上创建，
  // 当从std::shared_ptr上创建std::weak_ptr时两者指向相同的对象，
  // 但是std::weak_ptr不会影响所指对象的引用计数：
  std::weak_ptr<Widget> wpw(spw);

  std::cout << spw.use_count() << " " << wpw.use_count() << std::endl;  // 1 1
  spw = nullptr;  // 引用计数为0，Widget销毁，wpw现在悬空，被称作过期的
  std::cout << std::boolalpha << wpw.expired() << std::endl;            // true
  std::cout << spw.use_count() << " " << wpw.use_count() << std::endl;  // 0 0
}

void func2() {
  // 但是通常期望的是检查std::weak_ptr是否已经过期，如果没有过期则访问其指向的对象。
  // 这做起来可不是想着那么简单，因为缺少解引用操作，没有办法写这样的代码。
  // 即使有，将检查和解引用分开会引入竞态条件：在调用expired和解引用操作之间，
  // 另一个线程可能对指向这对象的std::shared_ptr重新赋值或者析构，并由此造成对象已析构。
  // 这种情况下，解引用将会产生未定义行为。

  std::cout.setf(std::ios_base::boolalpha);

  auto spw = std::make_shared<Widget>();
  std::weak_ptr<Widget> wpw(spw);
  spw = nullptr;
  // std::weak_ptr<Widget> wpw;  // 这样定义结果也一样

  // 需要的是一个原子操作检查std::weak_ptr是否已经过期，如果没有过期就访问所指对象。
  // 这可以通过从std::weak_ptr创建std::shared_ptr来实现，
  // 具体有两种形式可以从std::weak_ptr上创建std::shared_ptr：

  // 1.weak_ptr::lock，它返回一个shared_ptr，如果weak_ptr过期这个shared_ptr为空：
  std::shared_ptr<Widget> spw1 = wpw.lock();  // 如果wpw过期，spw1就为空
  auto spw2 = wpw.lock();                     // 同上，但是使用auto
  std::cout << (spw1 == nullptr) << std::endl;  // true
  std::cout << (spw2 == nullptr) << std::endl;  // true

  // 2.以weak_ptr为实参构造shared_ptr，这种情况中，如果weak_ptr过期，会抛出一个异常：
  try {
    std::shared_ptr<Widget> spw3(wpw);
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;  // bad_weak_ptr
  }
}

void func() {
  func1();
  func2();
}
}  // namespace n1

namespace n2 {
// 考虑一个工厂函数，它基于一个唯一ID从只读对象上产出智能指针。
// 根据条款18的描述，工厂函数会返回一个该对象类型的std::unique_ptr：
std::unique_ptr<const Widget> loadWidget(int id) {}
// 如果调用loadWidget是昂贵的操作，比如操作文件或数据库I/O，并且重复使用ID很常见，
// 一个合理的优化是再写一个函数除了完成loadWidget做的事情之外再缓存它的结果。
// 当每个请求获取的Widget阻塞了缓存也会导致本身性能问题，
// 所以另一个合理的优化可以是当Widget不再使用的时候销毁它的缓存。

// 对于可缓存的工厂函数，返回std::unique_ptr不是好的选择。
// 调用者应该接收缓存对象的智能指针，调用者也应该确定这些对象的生命周期，
// 但是缓存本身也需要一个指针指向它所缓存的对象，缓存对象的指针需要知道它是否已经悬空，
// 因为当工厂客户端使用完工厂产生的对象后，对象将被销毁，关联的缓存条目会悬空。
// 所以缓存应该使用std::weak_ptr，这可以知道是否已经悬空。
// 这意味着工厂函数返回值类型应该是std::shared_ptr，
// 因为只有当对象的生命周期由std::shared_ptr管理时，std::weak_ptr才能检测到悬空。
// 下面是一个临时凑合的loadWidget的缓存版本的实现：
std::shared_ptr<const Widget> fastLoadWidget(int id) {
  static std::unordered_map<int, std::weak_ptr<const Widget>> cache;

  // objPtr是去缓存对象的std::shared_ptr，当对象不在缓存中时为null：
  auto objPtr = cache[id].lock();

  if (!objPtr) {              // 如果不在缓存中
    objPtr = loadWidget(id);  // 加载它
    cache[id] = objPtr;       // 缓存它
  }
  return objPtr;

  // fastLoadWidget的缓存可能会累积过期的weak_ptr，这些指针对应了被销毁的对象。
}
}  // namespace n2

namespace n3 {
// 观察者模式主要组件是状态可能会更改的对象subjects和状态发生更改时要通知的对象observers。
// 在大多数实现中，每个subject都包含一个数据成员，该成员持有指向其observers的指针。
// 这使subjects很容易发布状态更改通知。
// subjects对控制observers的生命周期没有兴趣，但是subjects对确保另一件事具有极大的兴趣，
// 那事就是一个observer被销毁时，不再尝试访问它。
// 合理的设计是每个subject持有一个weak_ptrs容器指向observers，因此可以检查是否已经悬空。
}  // namespace n3

namespace n4 {
namespace test1 {
// A和C共享B的所有权：
// A --shared_ptr--> B <--shared_ptr-- C
// 需要从B指向A的指针，应该使用哪种指针？
// 1.原始指针，使用这种方法，如果A被销毁，但是C继续指向B，B就会有一个指向A的悬空指针。
//   而且B不知道指针已经悬空，所以B可能会继续访问，就会导致未定义行为；
// 2.std::shared_ptr，这种设计，A和B都互相持有对方的std::shared_ptr，
//   导致的std::shared_ptr环状结构（A指向B，B指向A）阻止A和B的销毁，
//   甚至A和B无法从其他数据结构访问了（比如，C不再指向B），每个的引用计数都还是1，
//   如果发生了这种情况，A和B都被泄漏，程序无法访问它们，但是资源并没有被回收；
// 3.std::weak_ptr，这避免了上述两个问题，如果A被销毁，B指向它的指针悬空，
//   但是B可以检测到这件事，尤其是，尽管A和B互相指向对方，B的指针不会影响A的引用计数，
//   因此在没有std::shared_ptr指向A时不会导致A无法被销毁。

// 使用weak_ptr显然是这些选择中最好的，但需要注意使用weak_ptr打破shared_ptr循环并不常见。
// 在严格分层的数据结构比如树中，子节点只被父节点持有，当父节点被销毁时，子节点就被销毁。
// 从父到子的链接关系可以使用std::unique_ptr很好的表征。
// 从子到父的反向连接可以使用原始指针安全实现，因为子节点的生命周期肯定短于父节点。
// 因此没有子节点解引用一个悬垂的父节点指针这样的风险。

// 当然，不是所有的使用指针的数据结构都是严格分层的，所以当发生这种情况时，
// 比如上面所述缓存和观察者列表的实现之类的，知道std::weak_ptr随时待命也是不错的。

// 从效率角度来看，std::weak_ptr与std::shared_ptr基本相同。
// 两者的大小是相同的，使用相同的控制块，构造、析构、赋值操作涉及引用计数的原子操作。
// 这可能让人感到惊讶，因为本条款开篇就提到std::weak_ptr不影响引用计数。
// 其实是std::weak_ptr不参与对象的共享所有权，因此不影响指向对象的引用计数。
// 实际上在控制块中还是有第二个引用计数，std::weak_ptr操作的是第二个引用计数。
}  // namespace test1

namespace test2 {
// shared_ptr循环引用是指两个对象相互使用shared_ptr指向对方，会造成内存泄漏。
class B;
class A {
 public:
  std::shared_ptr<B> bptr;
  void show() { std::cout << bptr.use_count() << std::endl; }
  ~A() { std::cout << "A is deleted" << std::endl; }
};
class B {
 public:
  std::shared_ptr<A> aptr;
  void show() { std::cout << aptr.use_count() << std::endl; }
  ~B() { std::cout << "B is deleted" << std::endl; }
};

void func1() {
  A* aptr = new A;
  B* bptr = new B;
  {
    std::shared_ptr<A> ap(aptr);
    std::shared_ptr<B> bp(bptr);
    ap->bptr = bp;
    bp->aptr = ap;

    // 在作用域内A和B的引用计数都为2：
    aptr->show();  // 2
    bptr->show();  // 2
  }
  // 退出循环时A的引用计数减1，B的引用计数也减1，但它们依旧不为0，引用计数均为1：
  aptr->show();  // 1
  bptr->show();  // 1

  // 对ap来说，只有调用了A的析构函数，才会去释放它的成员变量bptr。
  // 何时会调用A的析构函数呢？就是ap的引用计数为0。

  // 对于bp来说，只有调用了B的析构函数，才会去释放它的成员变量aptr。
  // 同样是bp的引用计数都为0的时候才能析构。

  // 现在，对于ap和bp来说，它们都拿着对方的share_ptr，有点类似于死锁的现象，
  // 没法使得ab和bp的引用计数为0，那么A和B的对象均无法析构，于是造成了内存泄漏。

  // ap和bp退出作用域了，为什么不会调用析构函数呢？
  // ap和bp是创建在栈上的，而不是A或者B对象的本身，ap、bp退出作用域，只是ap和bp本身释放了，
  // 只会使得A、B对象的引用计数-1，调用析构函数，是要A或B的对象，的引用计数为0才能执行析构函数。
}

void func2() {
  std::shared_ptr<A> ap(new A);
  std::shared_ptr<B> bp(new B);
  ap->bptr = bp;

  // 注意，顺序是反着的，先释放栈上的bp，然后再是ap：
  // A is deleted
  // B is deleted

  // 当ap和bp退出作用域时，首先栈上的bp会被释放，那么B对象的引用计数-1，从2变为1，
  // 然后栈上的ap会释放，那么A对象的引用计数减1，变成0，那么会调用A对象的析构函数，
  // 那么A对象的bptr也会被释放，那么B对象的引用计数减1，也变成0，就会调用B的析构函数。
}

void func3() {
  std::shared_ptr<A> ap(new A);
  std::shared_ptr<B> bp(new B);
  ap->bptr = bp;
  bp->aptr = ap;

  ap->bptr.reset();

  // B is deleted
  // A is deleted

  // 由于ap和bp都拿着对方的shared_ptr，导致循环引用，那么可以手动释放成员变量，
  // 比如将ap->bptr释放，那么此时B对象的引用计数为1，A对象的引用计数为2。
  // 就是跟func2类似的情况了，A对象和B对象都能够成功析构，不会造成内存泄漏。
}
}  // namespace test2

namespace test3 {
// 可以发现weak_ptr，不影响引用计数，是一种不控制对象生命周期的智能指针。
class B;
class A {
 public:
  std::weak_ptr<B> bptr;
  void show() { std::cout << bptr.use_count() << std::endl; }
  ~A() { std::cout << "A is deleted" << std::endl; }
};
class B {
 public:
  std::weak_ptr<A> aptr;
  void show() { std::cout << aptr.use_count() << std::endl; }
  ~B() { std::cout << "B is deleted" << std::endl; }
};

void func() {
  // weak_ptr解决循环引用问题：
  std::shared_ptr<A> ap(new A);
  std::shared_ptr<B> bp(new B);
  ap->bptr = bp;
  bp->aptr = ap;

  ap->show();  // 1
  bp->show();  // 1

  // B is deleted
  // A is deleted
}
}  // namespace test3
}  // namespace n4

// 要记住的东西：
// 1.用std::weak_ptr替代可能会悬空的std::shared_ptr；
// 2.std::weak_ptr的潜在使用场景包括：缓存、观察者列表、打破std::shared_ptr环状结构。

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
      n4::test2::func1();
      break;
    case 2:
      n4::test2::func2();
      break;
    case 3:
      n4::test2::func3();
      break;
    case 4:
      n4::test3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}