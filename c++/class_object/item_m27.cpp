#include <algorithm>
#include <iostream>
#include <list>
#include <string>

// 要求或禁止在堆中产生对象

// 有时你想这样管理某些对象，要让某种类型的对象能够自我销毁，
// 也就是能够delete this，很明显这种管理方式需要此类型对象被分配在堆中。
// 而其它一些时候你想获得一种保障：
// 不在堆中分配对象，从而保证某种类型的类不会发生内存泄漏。
// 如果在嵌入式系统上工作，就有可能遇到这种情况，
// 发生在嵌入式系统上的内存泄漏是极其严重的，其堆空间是非常珍贵的。

namespace n1 {
// 仅仅在堆中创建对象

namespace test1 {
// 禁止以调用new以外的其它手段建立对象，这很容易做到。
// 非堆对象在定义它的地方被自动构造，在生存时间结束时自动被释放，
// 所以只要禁止使用隐式的构造函数和析构函数，就可以实现这种限制。
// 把这些调用变得不合法的一种最直接的方法是：
// 把构造函数和析构函数声明为private，但这样做副作用太大。
class C {
 public:
  static C *create() { return new C(); }

 private:
  // 这种方法的缺点是一个类经常有许多构造函数，必须把它们都声明为private。
  // 否则如果这些函数就会由编译器生成，构造函数包括拷贝构造函数，
  // 也包括缺省构造函数，编译器生成的函数总是public：
  C() { std::cout << "C" << std::endl; };
  C(int i) {}
  C(double d) {}
  C(const C &c) {}
  ~C() { std::cout << "~C" << std::endl; };
};
void func() {
  // 无法创建对象：
  // C c1;
  // C *c2 = new C();
  // static C c3;

  C *c4 = C::create();
  // C
}
}  // namespace test1

namespace test2 {
// 没有理由让构造函数和析构函数都是private，
// 最好让析构函数成为private，让构造函数成为public。
// 仅仅声明析构函数为private是很简单的，因为每个类只有一个析构函数。
// 参考条款26，可以引进一个专用的伪析构函数，
// 用来访问真正的析构函数，客户端调用伪析构函数释放创建的对象。
// 注意，异常处理体系要求所有在栈中的对象的析构函数必须声明为公有。
class C {
 public:
  C() { std::cout << "C" << std::endl; };
  C(int i) {}
  C(double d) {}
  C(const C &c) {}
  // 伪析构函数，const成员函数，使得const对象也能被释放：
  void destroy() const { delete this; }

 private:
  ~C() { std::cout << "~C" << std::endl; };
};
void func1() {
  // C c1;  // 报错
  C *c2 = new C();
  // static C c3;  // 报错

  c2->destroy();

  // C
  // ~C
}

// 通过限制访问一个类的析构函数或它的构造函数来阻止建立非堆对象，
// 但是在条款26已经说过，这种方法也禁止了继承和包容（containment）：
class D : public C {};
class E {
 public:
  C c;
};
void func2() {
  // D d;  // 报错
  // E e;  // 报错
}

void func() {
  func1();
  func2();
}
}  // namespace test2

namespace test3 {
// 上面的困难不是不能克服的，通过把析构函数声明为protected，
// 同时它的构造函数还保持public，就可以解决继承的问题，
// 需要包含对象的类可以修改为包含指向对象的指针：
class C {
 public:
  C() { std::cout << "C" << std::endl; };
  void destroy() const { delete this; }

 protected:
  ~C() { std::cout << "~C" << std::endl; };
};
// 派生类能够访问protected成员：
class D : public C {};
class E {
 public:
  E() : c(new C) {}
  ~E() { c->destroy(); }
  C *c;
};
void func() {
  // C的构造函数不可能判断出它是否做为堆对象的基类而被调用，
  // 也就是说对于C的构造函数来说没有办法判断下面两种方式的区别：
  D *dp = new D;
  D d;  // d在栈上创建，包括基类部分

  E e;

  // C
  // C
  // ~C
  // ~C
}
}  // namespace test3
}  // namespace n1

namespace n2 {
// 判断一个对象是否在堆中

// 所有的对象，即使是做为其它派生类的基类也必须在堆中，如何能执行这种约束呢？
namespace test1 {
// 在operator new和new操作符调用的构造函数的相互作用中玩些小把戏：
// 当在堆上分配对象时，会调用operator new来分配raw memory，
// operator new设置onTheHeap为true，每个构造函数都会检测onTheHeap，
// 看对象的raw memory是否被operator new所分配。
class C {
 public:
  // 如果建立一个非堆对象，抛出一个异常：
  class HeapConstraintViolation {
   public:
    std::string what() const { return "HeapConstraintViolation"; }
  };
  static void *operator new(size_t size) {
    onTheHeap = true;
    return ::operator new(size);
  }
  C() {
    if (!onTheHeap) {
      throw HeapConstraintViolation();
    }
    onTheHeap = false;  // 为下一个对象清除标记
  }

 private:
  // 在构造函数内，指示对象是否被构造在堆上：
  static bool onTheHeap;
};
bool C::onTheHeap = false;

void func() {
  C *c1 = new C;

  try {
    C c2;
  } catch (const C::HeapConstraintViolation &e) {
    std::cout << e.what() << std::endl;
  }
  // HeapConstraintViolation

  try {
    // 第一个问题是为数组分配内存的是operator new[]，而不是operator new，
    // 不过能象编写operator new一样容易地编写operator new[]函数。
    // 更大的问题是numberArray有100个元素，所以会调用100次构造函数。
    // 但是只有一次分配内存的调用，
    // 所以100个构造函数中只有第一次调用构造函数前把onTheHeap设置为true。
    // 当调用第二个构造函数时，会抛出一个异常，真倒霉。
    C *c3 = new C[100];
  } catch (const C::HeapConstraintViolation &e) {
    std::cout << e.what() << std::endl;
  }
  // HeapConstraintViolation

  try {
    // new操作符调用两次，因此要调用两次operator new和调用两次构造函数。
    // 程序员一般期望这些函数以如下顺序执行：
    // 1.调用第一个对象的operator new
    // 2.调用第一个对象的构造函数
    // 3.调用第二个对象的operator new
    // 4.调用第二个对象的构造函数
    // 但是C++没有保证这就是它调用顺序，一些编译器以如下这种顺序生成函数调用：
    // 1.调用第一个对象的operator new
    // 2.调用第二个对象的operator new
    // 3.调用第一个对象的构造函数
    // 4.调用第二个对象的构造函数
    // 编译器生成这种代码丝毫没有错，
    // 但是在operator new中set-a-bit的技巧无法与这种编译器一起使用。
    // 因为在第一步和第二步设置的bit，第三步中被清除，
    // 那么在第四步调用对象的构造函数时，就会认为对象不再堆中，即使它确实在。
    C *c4 = new C(*new C);  // 先忽略内存泄漏
  } catch (const C::HeapConstraintViolation &e) {
    std::cout << e.what() << std::endl;  // 按分析应该抛出异常，但没有
  }
}
}  // namespace test1

namespace test2 {
// 如果陷入了极度绝望当中，可能会沦落进不可移植的领域里。
// 例如决定利用一个在很多系统上存在的事实，程序的地址空间被做为线性地址管理，
// 程序的栈从地址空间的顶部向下扩展，堆则从底部向上扩展。
// 在以这种方法管理程序内存的系统里（很多系统都是，但是也有很多不是这样），
// 可能会想能够使用下面这个函数来判断某个特定的地址是否在堆中：
bool isOnHeap(const void *address) {
  // 这个函数背后的思想很有趣，
  // 在onHeap函数中onTheSatck是一个局部变量，因此它在堆栈上。
  // 当调用onHeap时，它的栈框架（stack frame）被放在程序栈的顶端，
  // 因为栈在结构上是向下扩展的（趋向低地址），
  // onTheStack的地址肯定比任何栈中的变量或对象的地址小。
  // 如果参数address的地址小于onTheStack的地址，
  // 它就不会在栈上，而是肯定在堆上。
  char onTheStack;  // 局部栈变量
  return address < &onTheStack;
}
void func() {
  char *c1 = new char;  // 堆对象
  std::cout << "c1 is on Heap: " << std::boolalpha << isOnHeap(c1) << std::endl;
  char c2;  // 栈对象
  std::cout << "c2 is on Heap: " << std::boolalpha << isOnHeap(&c2)
            << std::endl;
  static char c3;  // 静态对象
  std::cout << "c3 is on Heap: " << std::boolalpha << isOnHeap(&c3)
            << std::endl;

  // c1 is on Heap: true
  // c2 is on Heap: false
  // c3 is on Heap: true

  // 最根本的问题是对象可以被分配在三个地方，而不是两个。
  // 是的，栈和堆能够容纳对象，但是忘了静态对象。
  // 静态对象是那些在程序运行时仅能初始化一次的对象。
  // 静态对象不仅仅包括显式的static对象，也包括在全局和命名空间里的对象。
  // 这些对象肯定位于某些地方，而这些地方既不是栈也不是堆。
  // 它们的位置是依据系统而定，但在很多栈和堆相向扩展的系统里，位于堆的底端。
  // 不能工作的原因立刻变得很清楚了，不能辨别堆对象与静态对象的区别。
}
}  // namespace test2

namespace test3 {
// 令人伤心的是不仅没有一种可移植的方法来判断对象是否在堆上，
// 而且连能在多数时间正常工作的准可移植的方法也没有。
// 如果实在非得必须判断一个地址是否在堆上，
// 必须使用完全不可移植的方法，其实现依赖于系统调用，只能这样做了。
// 因此你最好重新设计你的软件，以便可以不需要判断对象是否在堆中。
// 如果发现自己实在为对象是否在堆中这个问题所困扰，
// 一个可能的原因是想知道对象是否能在其上安全调用delete。
// 这种删除经常采用delete this这种声明狼籍的形式。
// 不过知道是否能安全删除一个指针，
// 与只简单地知道一个指针是否指向堆中的事物不一样，
// 因为不是所有在堆中的事物都能被安全地delete。

// 幸运的是判断是否能够删除一个指针比判断一个指针指向的事物是否在堆上要容易。
// 因为对于前者只需要一个operator new返回的地址集合。
// 因为能自己编写operator new函数（不能放到命名空间中）：
// void* operator new(size_t size) {
//   调用一些函数来分配内存：
//   void* p = getMemory(size);
//   处理内存不够的情况，把p加入到一个被分配地址的集合
//   return p;
// }
// void operator delete(void* ptr) {
//   releaseMemory(ptr);  // return memory to free store
//   从被分配地址的集合中移去ptr
// }
// bool isSafeToDelete(const void* address) {
//   返回address是否在被分配地址的集合中
// }

// 在实际当中，有三种因素制约着对这种设计方式的使用：
// 1.极不愿意在全局域定义任何东西，特别是那些已经具有某种含义的函数，
//   象operator new和operator delete。
//   只有一种具有正常特征形式的operator new和operator delete。
//   这样做会使得：
//   软件与其它实现全局版本的operator new和operator delete的软件不兼容。
// 2.第二个因素是效率：
//   如果不需要这些，为什么还要为跟踪返回的地址而负担额外的开销呢？
// 3.最后一点可能有些平常，但是很重要。
//   实现isSafeToDelete让它总能够正常工作是不可能的。
//   难点是多继承下来的类或继承自虚基类的类有多个地址，
//   所以无法保证传给isSafeToDelete的地址与operator new返回的地址相同，
//   即使对象在堆中建立。

// 希望提供这些功能时能够不污染全局命名空间，没有额外的开销，没有正确性问题。
// 幸运的是C++使用一种抽象mixin基类满足了需要。
// 抽象基类是不能被实例化的基类，也就是至少具有一个纯虚函数的基类。
// mixin(“mix in”)类提供某一特定的功能，
// 并可以与其继承类提供的其它功能相兼容，这种类几乎都是抽象类。
// 因此能够使用抽象混合（mixin）基类，
// 给派生类提供判断指针指向的内存是否由operator new分配的能力。
class HeapTracked {  // 混合类，跟踪从operator new返回的ptr
 public:
  class MissingAddress {
   public:
    std::string what() const { return "MissingAddress"; }
  };
  virtual ~HeapTracked(){};
  static void *operator new(size_t size) {
    void *memPtr = ::operator new(size);  // 获得内存
    addresses.push_front(memPtr);         // 把地址放到list的前端
    return memPtr;
  }
  static void operator delete(void *ptr) {
    // 得到一个iterator，用来识别list元素包含的ptr：
    std::list<RawAddress>::iterator it =
        std::find(addresses.begin(), addresses.end(), ptr);
    if (it != addresses.end()) {
      // 如果发现一个元素则删除该元素：
      addresses.erase(it);
      ::operator delete(ptr);  // 释放内存
    } else {
      // 否则ptr就不是用operator new分配的，所以抛出一个异常：
      throw MissingAddress();
    }
  }
  bool isOnHeap() const {
    // 得到一个指针，指向*this占据的内存空间的起始处：
    const void *rawAddress = dynamic_cast<const void *>(this);
    // 前面说过带有多继承或虚基类的对象会有几个地址，
    // 这导致编写全局函数isSafeToDelete会很复杂。
    // 这个问题在isOnHeap中仍然会遇到，
    // 但是因为isOnHeap仅仅用于HeapTracked对象中，
    // 能使用dynamic_cast操作符的一种特殊的特性来消除这个问题。
    // 只需简单地放入dynamic_cast，把一个指针dynamic_cast成void*类型，
    // const void*或volatile void*也可以，
    // 生成的指针将指向原指针指向对象内存的开始处。
    // 但是dynamic_cast只能用于指向至少具有一个虚拟函数的对象的指针上。
    // 该死的isSafeToDelete函数可以用于指向任何类型的指针，
    // 所以dynamic_cast也不能帮助它。
    // isOnHeap更具有选择性，它只能测试指向HeapTracked对象的指针，
    // 所以能把this指针dynamic_cast成const void*，
    // 变成一个指向当前对象起始地址的指针。
    // 如果HeapTracked::operator new为当前对象分配内存，
    // 这个指针就是HeapTracked::operator new返回的指针。
    // 如果你的编译器支持dynamic_cast操作符，这个技巧是完全可移植的。

    // 在operator new返回的地址list中查到指针
    std::list<RawAddress>::iterator it =
        find(addresses.begin(), addresses.end(), rawAddress);
    return it != addresses.end();  // 返回it是否被找到
  }

 private:
  typedef const void *RawAddress;
  static std::list<RawAddress> addresses;
};
std::list<HeapTracked::RawAddress> HeapTracked::addresses;

// 使用这个类，即使是最初级的程序员也可以在类中加入跟踪堆中指针的功能。
// 所需要做的就是让他们的类从HeapTracked继承下来。
// 想判断Assert对象指针指向的是否是堆对象：
class C : public HeapTracked {};
// 象HeapTracked这样的混合类有一个缺点，它不能用于内建类型，
// 因为象int和char这样的类型不能继承自其它类型。
// 不过使用象HeapTracked的原因一般都是要判断是否可以调用delete this，
// 你不可能在内建类型上调用它，因为内建类型没有this指针。

void func() {
  auto inventoryAsset = [](const C *c) {
    if (c->isOnHeap()) {
      std::cout << "on heap" << std::endl;
    } else {
      std::cout << "non heap" << std::endl;
    }
  };

  C c1;
  inventoryAsset(&c1);  // non heap
  static C c2;
  inventoryAsset(&c2);  // non heap
  C *c3 = new C;
  inventoryAsset(c3);  // on heap

  // try {
  //   delete (&c1);
  // } catch (const C::MissingAddress &e) {
  //   std::cout << e.what() << std::endl;
  // }

  // try {
  //   delete (&c2);
  // } catch (const C::MissingAddress &e) {
  //   std::cout << e.what() << std::endl;
  // }

  delete c3;
}
}  // namespace test3
}  // namespace n2

namespace n3 {
// 禁止创建堆对象

// 通常对象的创建分三种情况：
// 1.对象被直接实例化；
// 2.对象做为派生类的基类被实例化；
// 3.对象被嵌入到其它对象内。

// 禁止用户直接实例化堆象很简单，因为总是调用new创建，能够禁止用户调用new。
// 虽然不能影响new操作符的可用性（这是内嵌于语言的），
// 但是能够利用new操作符总是调用operator new函数这点，来达到目的。
// 可以自己声明这个函数，而且你可以把它声明为private：
class C {
 private:
  // 把operator new声明为private就足够了，
  // 但是把operator new声明为private，而把operator delete声明为public，
  // 这样做有些怪异，所以除非有绝对需要的原因，
  // 否则不要把它们分开声明，最好在类的一个部分里声明它们。
  // 如果你也想禁止堆对象数组，
  // 可以把operator new[]和operator delete[]也声明为private。
  static void *operator new(size_t size) {}
  static void operator delete(void *ptr) {}
};

// 把operator new声明为private，
// 经常会阻碍C对象做为一个位于堆中的派生类对象的基类被实例化。
// 因为operator new和operator delete是自动继承的，
// 如果operator new和operator delete没有在派生类中被声明为public，
// 它们就会继承基类中private的版本：
class D : public C {};
// 这个类没有声明operator new，如果派生类声明它自己的operator new，
// 当在堆中分配派生对象时，就会调用这个函数，
// 于是得另找一种不同的方法来防止基类C的分配问题。

// C的operator new是private这一点，
// 不会对包含C成员对象的对象的分配产生任何影响：
class E {
 private:
  C c;
};
// 又回到了这个问题上来，即如果C对象没有被构造在堆中，于是想抛出一个异常。
// 当然这次的问题是如果对象在堆中，抛出异常。
// 正像没有可移植的方法来判断地址是否在堆中一样，
// 也没有可移植的方法判断地址是否不在堆中，所以很不走运，
// 不过这也丝毫不奇怪，毕竟如果能辨别出某个地址在堆上，
// 也就能辨别出某个地址不在堆上。

void func() {
  C c1;
  static C c2;
  // C *c3 = new C;  // 编译错误

  D d1;
  static D d2;
  // D *d3 = new D;  // 编译错误

  // 正确，调用E::operator new或::operator new，
  // 不是C::operator new：
  E *e = new E;
}
}  // namespace n3

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 6]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::test1::func();
      break;
    case 1:
      n1::test2::func();
      break;
    case 2:
      n1::test3::func();
      break;
    case 3:
      n2::test1::func();
      break;
    case 4:
      n2::test2::func();
      break;
    case 5:
      n2::test3::func();
      break;
    case 6:
      n3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}