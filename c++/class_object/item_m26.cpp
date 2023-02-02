#include <iostream>
#include <string>

// 限制某个类所能产生的对象数量

namespace n1 {
namespace test1 {
// 每次创建对象都要调用构造函数，如果想要阻止某个类创建对象，
// 只要把构造函数放到private就可以了：
class C {
 private:
  C();
  C(const C &c);
};
void func() {
  // 这样谁都没有权力建立对象：
  // C c1;
  // static C c2;
  // C *c3 = new C;
}
}  // namespace test1

namespace test2 {
// 可以有选择性地放松这个限制，这个设计由三个部分组成：
// 1.C类的构造函数是private，这样能阻止建立对象；
// 2.全局函数CreateC被声明为类的友元，
//   让CreateC避免私有构造函数引起的限制；
// 3.最后CreateC包含一个静态C对象，这意味着只有一个对象被建立。
class C {
 public:
  friend C &CreateC();

 private:
  C() { std::cout << "C()" << std::endl; }
  C(const C &c) { std::cout << "C(const C &c)" << std::endl; }
};
C &CreateC() {
  static C c;
  return c;
}
void func() {
  // 代码无论何时要使用类C的对象，都要使用CreateC函数：
  CreateC();
  CreateC() = CreateC();

  // C()
}
}  // namespace test2

namespace test3 {
// CreateC使用全局命名空间完全是多余的，想把创建对象的功能放到类里。
// 这很简单，只要在类C中声明CreateC为静态函数，就不再需要友元声明了：
class C {
 public:
  static C &CreateC() {
    static C c;
    return c;
  }

 private:
  C() { std::cout << "C()" << std::endl; }
  C(const C &c) { std::cout << "C(const C &c)" << std::endl; }
};
void func() {
  C::CreateC();
  C::CreateC() = C::CreateC();

  // C()
}
}  // namespace test3

namespace test4 {
// 另一种方法是把CreateC移出全局域，放入namespace（命名空间）。
// 命名空间是C++一个较新的特性，
// 任何能在全局域声明东西也能在命名空间里声明，
// 包括类、结构、函数、变量、对象、typedef等等。
// 把它们放入命名空间并不影响它们的行为特性，
// 不过能够防止在不同命名空间里的实体发生命名冲突。
// 命名空间从句法上来看有些象类，
// 但是它没有public、protected或private域，所有都是public。
namespace NC {
class C {
 public:
  friend C &CreateC();

 private:
  C() { std::cout << "C()" << std::endl; }
  C(const C &c) { std::cout << "C(const C &c)" << std::endl; }
};
C &CreateC() {
  static C c;
  return c;
}
}  // namespace NC
void func() {
  NC::CreateC();
  using NC::CreateC;
  CreateC() = CreateC();

  // C()
}
}  // namespace test4

void func() {
  n1::test1::func();
  n1::test2::func();
  n1::test3::func();
  n1::test4::func();
}

// 在CreateC的实现上有两个微妙的不引人注目的地方，值得看一看：
// 1.唯一的对象是函数里的静态变量而不是在类中的静态成员，这是非常重要的。
//   首先：
//   在类中的静态对象实际上总是被构造（和释放），即使不使用该对象。
//   与此相反，只有第一次执行函数时，才会建立函数中的静态变量，
//   所以如果没有调用函数，就不会建立对象。
//   不过得为此付出代价，每次调用函数时都得检查是否需要建立对象。
//   其次：
//   函数的静态变量在函数第一次执行时初始化，
//   类的静态成员的初始化时间不确定。
//   C++为一个编译单元内的静态成员的初始化顺序提供某种保证，
//   但是对于在不同编译单元中的静态成员的初始化顺序则没有这种保证。
//   在实际使用中，类的静态成员的这个缺点会带来许多麻烦。
// 2.另一个细微之处是内联与函数内静态对象的关系。
//   CreateC的非成员函数形式，除了第一次执行这个函数时（也就是构造p时），
//   其它时候这就是一个一行函数，它由return p;一条语句组成。
//   这个函数最适合做为内联函数使用，然而它不能被声明为内联，为什么呢？
//   为什么你要把对象声明为静态呢？通常是因为你只想要该对象的一个拷贝。
//   现在再考虑内联意味着什么呢？
//   从概念上讲，它意味着编译器用函数体替代该对函数的每一个调用，
//   不过非成员函数还不只这些。
//   非成员函数还有其它的含义，它还意味着internal linkage（内部链接）。
//   通常情况下，不需要理解这种语言上令人迷惑的东西，只需记住一件事：
//   带有内部链接的函数可能在程序内被复制，
//   也就是说程序的object代码可能包含一个以上的内部链接函数的代码，
//   这种复制也包括函数内的静态对象。
//   结果如何？如果建立一个包含局部静态对象的非成员函数，
//   可能会使程序的静态对象的拷贝超过一个！
//   所以不要建立包含局部静态数据的非成员函数。
}  // namespace n1

namespace n2 {
// 但是可能认为建立函数来返回一个隐藏对象的引用，
// 这种限制对象的数量的方法是错误的。
// 也许有人认为只需简单地计算对象的数目，一旦需要太多的对象，
// 就抛出异常，这样做也许会更好。
class C {
  // 这种限制建立对象数目的方法有两个较吸引人的优点：
  // 1.它是直观的，每个人都能理解它的用途；
  // 2.很容易推广它的用途，可以允许建立对象最多的数量不是一，
  //   而是其它大于一的数字。
 public:
  C() {
    if (num_objects_ >= 1) {
      throw TooManyObjects();
    }
    ++num_objects_;
    std::cout << "C" << std::endl;
  }
  ~C() {
    std::cout << "~C" << std::endl;
    --num_objects_;
  }
  class TooManyObjects {
   public:
    std::string what() const { return "TooManyObjects"; }
  };

 private:
  static int num_objects_;
  C(const C &c);
};
int C::num_objects_ = 0;
void func1() {
  C c1;
  try {
    C c2;
  } catch (const C::TooManyObjects &e) {
    std::cout << e.what() << std::endl;
  }

  // C
  // TooManyObjects
  // ~C
}

// 问题：
// 1.派生类对象也会包含类C的对象，会抛出异常；
class D : public C {};
void func2() {
  C c;
  try {
    D d;
  } catch (const C::TooManyObjects &e) {
    std::cout << e.what() << std::endl;
  }

  // C
  // TooManyObjects
  // ~C
}
// 2.当其它对象包含类C的对象时，也会抛出异常。
class E {
 private:
  C c;
};
void func3() {
  C c;
  try {
    E e;
  } catch (const C::TooManyObjects &e) {
    std::cout << e.what() << std::endl;
  }
}

// 问题是C对象能存在于三种不同的环境中：
// 1.只有它们本身；
// 2.作为其它派生类的基类；
// 3.被嵌入在更大的对象里。
// 存在这些不同环境极大地混淆了跟踪存在对象的数目的含义，
// 因为你心目中的对象的存在含义与编译器不一致。
}  // namespace n2

namespace n3 {
// 通常开发者仅会对允许对象本身存在的情况感兴趣，
// 希望限制这种实例（instantiation）的数量。
// 如果使用n1中示例的方法，就很容易进行这种限制，因为构造函数是private，
// 不存在friend声明的带有private构造函数的类不能作为基类使用，
// 也不能嵌入到其它对象中。
// 不能从带有private构造函数的类派生出新类，
// 这个事实导致产生了一种阻止派生类的通用方法，
// 这种方法不需要和限制对象实例数量的方法一起使用。

namespace test1 {
// 假设允许建立任意数量的对象，但是想禁止派生出新类：
class C {
 public:
  // 伪构造函数：
  static C *CreateC() { return new C; }
  static C *CreateC(const C &c) { return new C(c); }

 private:
  C() { std::cout << "C()" << std::endl; }
  C(const C &c) { std::cout << "C(const C &c)" << std::endl; }
};
class D : public C {};
class E {
  C c;
};
void func() {
  C *c1 = C::CreateC();
  C *c2 = C::CreateC();
  C *c3 = C::CreateC(*c1);
  // C()
  // C()
  // C(const C &c)

  // D d;  // 报错
  // E e;  // 报错
}
}  // namespace test1

namespace test2 {
// 把现在对象个数与伪构造函数结合起来：
class C {
 public:
  ~C() {
    std::cout << "~C" << std::endl;
    --num_objects_;
  }
  static C *CreateC() { return new C; }
  class TooManyObjects {
   public:
    std::string what() const { return "TooManyObjects"; }
  };

 private:
  static int num_objects_;
  C() {
    if (num_objects_ >= 1) {
      throw TooManyObjects();
    }
    ++num_objects_;
    std::cout << "C" << std::endl;
  }
  C(const C &c);
};
int C::num_objects_ = 0;
void func() {
  C *c1 = C::CreateC();
  try {
    C *c2 = C::CreateC();
  } catch (const C::TooManyObjects &e) {
    std::cout << e.what() << std::endl;
  }
  delete c1;
  C *c3 = C::CreateC();
  delete c3;

  // C
  // TooManyObjects
  // ~C
  // C
  // ~C

  // C c4 = *c1;  // 拷贝构造函数是private，报错
}
}  // namespace test2

namespace test3 {
// 推广到现在任意数量的对象：
class C {
 public:
  ~C() {
    std::cout << "~C" << std::endl;
    --num_objects_;
  }
  static C *CreateC() { return new C; }
  static C *CreateC(const C &c) { return new C(c); }
  class TooManyObjects {
   public:
    std::string what() const { return "TooManyObjects"; }
  };

 private:
  static int num_objects_;
  static const int max_objects_;
  C() {
    if (num_objects_ >= max_objects_) {
      throw TooManyObjects();
    }
    ++num_objects_;
    std::cout << "C" << std::endl;
  }
  C(const C &c) {
    if (num_objects_ >= max_objects_) {
      throw TooManyObjects();
    }
    ++num_objects_;
    std::cout << "C(const C &c)" << std::endl;
  }
};
int C::num_objects_ = 0;
const int C::max_objects_ = 4;
class D : public C {};
class E {
  C c;
};
void func() {
  C *c1 = C::CreateC();
  C *c2 = C::CreateC();
  C *c3 = C::CreateC();
  C *c4 = C::CreateC(*c2);
  try {
    C *c5 = C::CreateC();
  } catch (const C::TooManyObjects &e) {
    std::cout << e.what() << std::endl;
  }
  delete c1;
  C *c6 = C::CreateC();
  delete c2;
  delete c3;
  delete c4;
  delete c6;

  // C
  // C
  // C
  // C(const C &c)
  // TooManyObjects
  // ~C
  // C
  // ~C
  // ~C
  // ~C
  // ~C

  // D d;  // 报错
  // E e;  // 报错
}
}  // namespace test3

namespace test4 {
// 一个具有对象计数功能的基类：
template <class BeingCounted>
class Counted {
 public:
  class TooManyObjects {
   public:
    std::string what() const { return "TooManyObjects"; }
  };
  static int objectCount() { return numObjects; }

 protected:
  Counted() { init(); }
  Counted(const Counted &rhs) { init(); }
  ~Counted() { --numObjects; }

 private:
  static int numObjects;
  static const size_t maxObjects;
  void init() {
    if (numObjects >= maxObjects) {
      throw TooManyObjects();
    }
    ++numObjects;
  }
};
template <class BeingCounted>
int Counted<BeingCounted>::numObjects;

class C : private Counted<C> {
 public:
  ~C() { std::cout << "~C" << std::endl; }
  static C *CreateC() { return new C; }
  static C *CreateC(const C &c) { return new C(c); }
  using Counted<C>::objectCount;
  // 老式的访问权声明语法，让objectCount在C中是public：
  // Counted<C>::objectCount;
  using Counted<C>::TooManyObjects;

 private:
  C() { std::cout << "C" << std::endl; }
  C(const C &c) { std::cout << "C(const C &c)" << std::endl; }
};
// 基类模板不对maxObject进行初始化，而是让调用者提供合适的初始化：
template <>
const size_t Counted<C>::maxObjects = 4;  // 如果没有，链接错误
class D : public C {};
class E {
  C c;
};

void func() {
  C *c1 = C::CreateC();
  C *c2 = C::CreateC();
  C *c3 = C::CreateC(*c1);
  C *c4 = C::CreateC(*c2);
  std::cout << C::objectCount() << std::endl;
  try {
    C *c5 = C::CreateC();
  } catch (const C::TooManyObjects &e) {
    std::cout << e.what() << std::endl;
  }
  delete c1;
  std::cout << C::objectCount() << std::endl;
  C *c6 = C::CreateC();
  delete c2;
  C *c7 = C::CreateC(*c3);
  delete c3;
  delete c4;
  delete c6;
  delete c7;
  std::cout << C::objectCount() << std::endl;

  // C
  // C
  // C(const C &c)
  // C(const C &c)
  // 4
  // TooManyObjects
  // ~C
  // 3
  // C
  // ~C
  // C(const C &c)
  // ~C
  // ~C
  // ~C
  // ~C
  // 0

  // D d;  // 报错
  // E e;  // 报错
}

// 类C使用了Counter模板来跟踪存在多少类C对象，
// 坦率地说，除了类C的编写者，没有人关心这个事实。
// 它的实现细节最好是private，这就是为什么这里使用private继承的原因。
// 另一种方法是在C和counted<C>之间使用public继承，
// 但是必须给Counted类一个虚拟析构函数。
// 否则如果有人通过Counted<C>*指针删除一个类C对象，
// 就有导致对象行为不正确的风险。
// 在Counted中存在虚函数，几乎肯定影响从Counted继承的对象的大小和布局。
// 不想引入这些额外的负担，所以使用private继承来避免这些负担。
// Counted所做的大部分工作对于类C的用户来说都是隐藏的，
// 但是这些用户可能很想知道有当前多少类C对象存在。
// Counted模板提供了objectCount函数，用来提供这种信息，
// 但是因为我们使用private继承，这个函数在C类中成为了private。
// 当C继承Counted<C>时，它可以忘记有关对象计数的事情。
// 编写C类时根本不用考虑对象计数，就好像有其他人会为它计数一样。
// C的构造函数可以是这样的：C::C() {}
// 这里有趣的不是所见到的东西，而是看不到的东西。
// 不检测对象的数量就好像限制将被超过，
// 执行完构造函数后也不增加存在对象的数目。
// 所有这些现在都由Counted<C>的构造函数来处理，
// 因为Counted<C>是类C的基类，
// 所以Counted<C>的构造函数总在类C的前面被调用。
}  // namespace test4
}  // namespace n3

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 7]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::func1();
      break;
    case 2:
      n2::func2();
      break;
    case 3:
      n2::func3();
      break;
    case 4:
      n3::test1::func();
      break;
    case 5:
      n3::test2::func();
      break;
    case 6:
      n3::test3::func();
      break;
    case 7:
      n3::test4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}