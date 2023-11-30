#include <iostream>
#include <memory>
#include <mutex>

// 为异常安全而努力是值得的

// 当异常被抛出时，带有异常安全性的函数会：
// 1.不泄露任何资源；
// 2.不允许破坏数据。

// 异常安全函数提供以下三个保证之一：
// 1.基本承诺
//   如果异常抛出，程序内的任何事物仍然保持在有效状态下。
//   没有任何对象或数据结构会因此而破坏，所有对象都处于一种内部前后一致的状态。
//   然后程序的显示状态恐怕不可预料。
//   也就是说程序可能处于任何状态，只要那个是个合法状态。
//   对于下面的例子，PrettyMenu可以继续拥有原背景图像，
//   或是令它拥有某个缺省背景图像，但客户无法预期是哪一种。
// 2.强烈保证
//   如果异常被抛出，程序状态不改变，调用这样的函数需要有这样的认知：
//   如果函数成功，就是完全成功，
//   如果函数失败，程序会回复到调用函数之前的状态。
// 3.不抛出保证
//   承诺绝不抛出异常，因为它们总是能够完成原先承诺的功能。
//   作用于内置类型身上的所有操作都提供nothrow保证。
//   这是异常安全码中一个必不可少的关键基础材料。
//   函数带着空白的异常明细者必为nothrow函数，似乎合情合理，其实不尽然。

// 并不是说doSomething绝不会抛出异常，而是如果抛出异常，将是严重错误：
int doSomething() throw();
// 函数的声明（包括异常明细）并不能告诉你是否它正确的、可移植的或高效的，
// 也不能告诉你是否提供任何异常安全性保证。
// 所有的性质都由函数的实现决定，与声明无关。
// 异常安全代码必须提供上述三种保证之一，如果不这样做，它就不具备异常安全性。

// 一般而言应该会提供可实现的最强烈保证。
// nothrow很好，但是很难在C part of
// C++中，完全没有调用任何一个可能抛出异常的函数。
// 任何使用动态内存的东西（STL容器）如果无法找到足够内存，通常会抛出bad_alloc异常。
// 可能的话请提供nothrow保证，但对大部分函数而言，抉择往往落在基本保证和强烈保证之间。

struct Image {
  Image(std::istream& imgSrc){};
  void reset(std::istream& imgSrc){};
};
struct PMImpl {
  // 使用struct而不是class，是因为pImpl已经是私有的了，
  // 如果用class（默认是private）不太方便，不能直接访问成员。
  int imageChagnes;
  std::shared_ptr<Image> bgImage;
};
class PrettyMenu {
 public:
  // 异常安全的两个条件，这个函数都没满足：
  void changeBackground1(std::istream& imgSrc) {
    m.lock();
    delete bgImage;
    ++imageChagnes;
    // new Image抛出异常，bgImage指向一个已被删除的对象，
    // imageChagnes也已被累加，而其实并没有新图像被加载。
    bgImage = new Image(imgSrc);
    // new Image发生异常，unlock不会被调用，死锁：
    m.unlock();
  }

  // 解决资源泄露问题很容易，使用条款13描述的以对象管理资源：
  void changeBackground2(std::istream& imgSrc) {
    std::lock_guard<std::mutex> lg(m);
    delete bgImage;
    ++imageChagnes;
    bgImage = new Image(imgSrc);
  }

  // 提供强烈保证几乎不困难：
  void changeBackground3(std::istream& imgSrc) {
    std::lock_guard<std::mutex> lg(m);
    // 如果Image构造函数抛出异常，有可能输入流的读取记号已被移走，
    // 而这样的搬移对程序其余部分是一种可见的状态改变。
    // 在解决这个问题之前只提供基本的异常安全保证。
    bgImage1.reset(new Image(imgSrc));
    ++imageChagnes;
  }

  // 有一个一般化的设计策略很典型地会导致强烈保证，叫做copy and swap。
  // 原则很简单：
  // 1.为打算修改的对象进行复制；
  // 2.在副本上做一切必要的修改，若有任何修改动作抛出异常，原对象仍保持未改变状态；
  // 3.待所有改变都成功后，再将修改过的那个副本和对象在一个不抛出异常的操作中交换。
  // 实现上通常是将所有隶属对象的数据从原对象放进另一个对象，然后赋予原对象一个指针，
  // 指向哪个所谓的实现对象，这种手法常别称为pimpl idiom(条款31)。
  void changeBackground4(std::istream& imgSrc) {
    using std::swap;
    std::lock_guard<std::mutex> lg(m);
    std::shared_ptr<PMImpl> pNew(new PMImpl(*pImpl));
    pNew->bgImage.reset(new Image(imgSrc));
    ++pNew->imageChagnes;
    swap(pImpl, pNew);
  }

 private:
  std::mutex m;
  Image* bgImage;
  std::shared_ptr<Image> bgImage1;  // 强烈保证使用智能指针
  std::shared_ptr<PMImpl> pImpl;  // 强烈保证的一般化设计，pimpl idiom
  int imageChagnes;
};

// copy and swap策略是对对象状态做出全有或全无改变的一个很好办法，
// 但一般而言它并不保证整个函数有强烈的异常安全性：
void someFunction() {
  // 对local状态做一份副本
  // f1()
  // f2()
  // 将修改后的状态置换过来
}
// 如果f1或f2的异常安全性比强烈保证低，就很难让someFunc成为强烈异常安全。
// 如果f1或f2都是强烈异常安全，情况并不就此好转。
// 比较如果f1圆满结束，程序状态在任何方面都可能有所改变，
// 此时f2随后抛出异常，程序状态和someFunc被调用前并不相同，
// 甚至当f2没有改变任何东西时也是如此。
// 如果f1修改了数据库（没有什么做法可以取消并恢复数据库），
// 那就很难让someFunc具备强烈安全性。
// 另一个问题是效率，copy and swap会消耗很多时间和空间。

// 当强烈保证不切实际时，就必须提供基本保证，现实中会发现，可以提供强烈保证，
// 但效率和复杂度带来的成本会使你退而求其次，只提基本保证，任何人都不该因此责怪你，
// 对许多函数而言，异常安全性的基本保证是一个绝对通情达理的选择。

// 如果具备异常安全性：
// 首先是以对象管理资源，可阻止资源泄露，然后挑选三个异常安全保证中的某一个实施。
// 应该挑选可实现的最强烈等级，只有当调用了传统代码，才别无选择地设为无任何保证。

// 请记住：
// 1.异常安全函数即使发生异常也不会泄露资源或允许任何数据结果被破坏；
// 2.这样的函数区分为三种可能的保证：基本型、强烈型、不抛异常型；
// 3.强烈保证往往能够以cas实现出来，但强烈保证并非对所有函数都可实现或具备现实意义；
// 4.函数提供的异常安全保证通常最高只等于其所调用的各个函数的异常安全保证中的最弱者。

int main() { return 0; }