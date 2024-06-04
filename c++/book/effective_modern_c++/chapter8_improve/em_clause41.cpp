#include <iostream>
#include <memory>
#include <string>
#include <vector>

// 条款41：针对可复制的形参，在移动成本低并且一定会被复制的前提下，考虑将其按值传递

class A {
 public:
  A() = default;
  A(const A&) { std::cout << "A(const A&)" << std::endl; }
  A(A&&) { std::cout << "A(A&&)" << std::endl; }
};

namespace n1 {
// 有些函数的形参本来就是打算拿来复制的。
// 例如，成员函数addName可能会将其形参复制入其私有容器。
// 为效率计，这样的函数应该针对左值实参实施复制，而针对右值实参实施移动：
class Widget {
 public:
  // 如果没有，会扩容：
  Widget() { as.reserve(2); }

  // 这样写也没有错，但是要求撰写本质上在做同一件事的两个函数，这么一来就有活干了：
  // 需要撰写两份函数声明、两份函数实现、两份函数文档、两份函数维护工作量，要命。
  void addA(const A& a) { as.push_back(a); }
  void addA(A&& a) { as.push_back(std::move(a)); }
  // 在目标代码中将出现两个函数，如果确实在意程序足迹一事，
  // 这一情况就可能会让人忧心，在本例情况下，这两个函数都可能会被实施内联，
  // 而这很可能会消除与存在两个函数相关的任何膨胀问题，
  // 但是如果这些函数没有处处实施内联的话，那么在目标代码中就真会看到有两个函数了。

 private:
  std::vector<A> as;
};

void func() {
  Widget w;

  A a;
  w.addA(a);  // A(const A&)

  w.addA(A());  // A(A&&)

  // 使用重载，无论传递左值还是传递右值，调用都会绑定到一个叫a的引用上。
  // 从拷贝和移动操作方面看，这个过程零开销。
  // 左值重载中，a拷贝到Widget::as中，右值重载中，移动进去。
  // 开销总结：左值一次拷贝，右值一次移动。
}
}  // namespace n1

namespace n2 {
class Widget {
 public:
  Widget() { as.reserve(2); }

  // 另一种方法是把addA写成接受万能引用的函数模板：
  template <typename T>
  void addA(T&& a) {
    as.push_back(std::forward<T>(a));
  }
  // 这减少了需要处理的源代码数量，但是万能引用的使用会导致其他方面的复杂性。
  // 作为模板，addA的实现通常必须置于头文件中，它还可能在对象代码中产生好几个函数，
  // 因为它不仅针对左值和右值会产生不同的实例化效果，
  // 针对A和可以转换为A的类型（条款25）也会产生不同的实例化结果。
  // 同时，有些类型不能按通用引用方式传递（条款30），如果客户传入了不正确的实参类型，
  // 编译器错误消息可能会吓人一跳（条款27）。

 private:
  std::vector<A> as;
};

void func() {
  Widget w;

  A a;
  w.addA(a);  // A(const A&)

  w.addA(A());  // A(A&&)

  // 使用通用引用，同重载一样，调用也绑定到addName这个引用上，没有开销。
  // 由于使用了std::forward，左值a实参会拷贝到Widget::as，右值实参移动进去。
  // 对a实参来说，开销同重载方式一样：左值一次拷贝，右值一次移动。

  // Item25解释了如果调用者传递的实参不是A类型，将会转发到A的构造函数，
  // 几乎也没有A的拷贝或者移动操作，因此通用引用的方式有同样效率，
  // 所以这不影响本次分析，简单假定调用者总是传入A类型实参即可。
}
}  // namespace n2

namespace n3 {
// 如果有一种方法来撰写像addName这样的函数，针对左值实施的是复制，
// 针对右值实施的是移动，而且无论在源代码和目标代码中只有一个函数需要着手处理，
// 还能避免万能引用的怪癖，那该多妙，无巧不成书，方法还真有一个，
// 所需要做的事情只有一件，就是要放弃你可能身为C++程序员学到的第一个条规则。
// 该规则说，要避免按值传递用户定义类型的对象。
// 对于像addA这样的函数中的A这样的形参，按值传递可能是个完全合理的策略。

class Widget {
 public:
  Widget() { as.reserve(2); }
  void addA(A a) {
    // std::move典型的应用场景是用在右值引用，但是在这里：
    // 1.a是与调用者传进来的对象相完全独立的一个对象，所以改变a不会影响到调用者；
    // 2.这就是a的最后一次使用，所以移动它不会影响函数内此后的其他代码。
    as.push_back(std::move(a));
  }
  // 只有一个addA函数的事实，解释了在源代码和目标代码中，怎样避免了代码重复。
  // 没有使用通用引用，不会导致头文件膨胀、奇怪的失败情况、或者令人困惑的编译错误消息。
  // 但是这种设计的效率如何呢？在按值传递哎，会不会开销很大？
  // 在C++98中，可以肯定确实开销会大，无论调用者传入什么，形参a都是由拷贝构造出来。
  // 但是在C++11中，只有在左值实参情况下，a被拷贝构造出来，对于右值，它会被移动构造。

 private:
  std::vector<A> as;
};

void func() {
  Widget w;

  A a;
  w.addA(a);
  // A(const A&)
  // A(A&&)

  w.addA(A());

  // g++ -std=c++11 em_clause41.cpp
  // A(A&&)

  // g++ -std=c++11 em_clause41.cpp -fno-elide-constructors
  // A(A&&)
  // A(A&&)

  // 按值传递，无论传递左值还是右值，都必须构造newName形参。
  // 如果传递的是左值，需要拷贝的开销，如果传递的是右值，需要移动的开销。
  // 在函数的实现中，a总是采用移动的方式到Widget::as。
  // 开销总结：左值实参，一次拷贝一次移动，右值实参两次移动。
  // 对比按引用传递的方法，对于左值或者右值，均多出一次移动操作。
}
}  // namespace n3

namespace n4 {
// 对于移动成本低且总是被拷贝的可拷贝形参，考虑按值传递，有四个原因：

// 1.应该仅考虑使用传值方式，只需要编写一个函数，目标代码中也只生成一个函数，
//   避免了通用引用的种种问题，但是开销会比指接受引用的两种实现方式更高；

// 2.仅考虑对于可拷贝形参使用按值传递，不符合此条件的的形参必须有只可移动的类型，
//   因为函数总是会做副本，而如果它们不可拷贝，副本就必须通过移动构造函数创建，
//   回忆一下传值方案比重载方案的优势在于，仅有一个函数要写，但是对于只可移动类型，
//   没必要为左值实参提供重载，因为拷贝左值需要拷贝构造函数，
//   只可移动类型的拷贝构造函数是禁用的，那意味着只需要支持右值实参，
//   重载方案只需要一个重载函数，接受右值引用的函数；

// 考虑一个类：有个std::unique_ptr<std::string>的数据成员，对它有个setter。
// std::unique_ptr是只可移动的类型，所以赋值器的重载方式只有一个函数：
class Widget {
 public:
  void setPtr1(std::unique_ptr<std::string>&& ptr) { p = std::move(ptr); }
  void setPtr2(std::unique_ptr<std::string> ptr) { p = std::move(ptr); }

 private:
  std::unique_ptr<std::string> p;
};

void func() {
  Widget w;

#if __cplusplus == 201402L
  w.setPtr1(std::make_unique<std::string>("Modern C++"));
#elif __cplusplus == 201103L
  w.setPtr2(std::unique_ptr<std::string>(new std::string("Modern C++")));
#endif
  // 从std::make_unique返回的右值std::unique_ptr<std::string>，
  // 通过右值引用被传给setPtr，然后移动到数据成员p中，整体开销就是一次移动。

  // 同样的调用，setPtr2就会先移动构造ptr形参，然后ptr再移动赋值到数据成员p，
  // 整体开销就是两次移动，是重载方法开销的两倍。
}

// 3.按值传递应该仅考虑那些移动开销小的形参，当移动的开销较低，
//   额外的一次移动才能被开发者接受，但是当移动的开销很大，
//   执行不必要的移动就类似执行一个不必要的拷贝，
//   而避免不必要的拷贝的重要性就是最开始C++98规则中避免传值的原因；

// 4.应该只对总是被拷贝的形参考虑按值传递，为了看清楚为什么这很重要，
//   假定在拷贝形参到as容器前，addA需要检查新的名字的长度是否过长或者过短，
//   如果是，就忽略增加名字的操作：
//   class Widget {
//    public:
//     void addName(std::string newName) {
//       if ((newName.length() >= minLen) && (newName.length() <= maxLen)) {
//         names.push_back(std::move(newName));
//       }
//     }
//    private:
//     std::vector<std::string> names;
//   };
//   即使这个函数没有在names添加任何内容，也增加了构造和销毁newName的开销，
//   而按引用传递会避免这笔开销。
}  // namespace n4

namespace n5 {
// 即使对可拷贝类型执行无条件的复制，且这个类型移动开销小，有时也可能不适合按值传递。
// 这是因为函数拷贝形参存在两种方式：通过构造函数（拷贝或移动）和赋值（拷贝或移动）。
// addName使用构造函数，它的形参newName传递给vector::push_back，
// 在这个函数内部，newName是通过拷贝构造在std::vector末尾创建一个新元素。
// 对于使用构造函数拷贝形参的函数，之前的分析已经可以给出最终结论：
// 按值传递对于左值和右值均增加了一次移动操作的开销。

namespace test1 {
// 当形参通过赋值操作进行拷贝时，分析起来更加复杂。
// 比如，有一个表示密码的类，因为密码可能会被修改，提供了赋值器函数changeTo。
// 用按值传递的策略，实现一个Password类如下：
class Password {
 public:
  explicit Password(std::string pwd) : text(std::move(pwd)) {}
  void changeTo(std::string newPwd) { text = std::move(newPwd); }

 private:
  std::string text;
};

void func() {
  std::string initPwd("Supercalifragilisticexpialidocious");
  Password p(initPwd);
  // p.text被给定的密码构造，在构造函数用按值传递的方式增加了一次移动构造的开销，
  // 如果使用重载或者通用引用就会消除这个开销，一切都还好。

  std::string newPassword = "Beware the Jabberwock";
  p.changeTo(newPassword);
  // changeTo使用赋值来拷贝形参newPwd，可能导致按值传递实现方案的开销大大增加。

  // 传递给changeTo的实参是一个左值newPassword，所以newPwd形参被构造时，
  // std::string的拷贝构造函数会被调用，这个函数会分配新的存储空间给新密码。
  // newPwd会移动赋值到text，这会导致text本来持有的内存被释放。
  // 所以changeTo存在两次动态内存管理的操作，为新密码创建内存和销毁旧密码的内存。
}
}  // namespace test1

namespace test2 {
// 如果使用重载的方式，有可能两次动态内存管理操作得以避免：
class Password {
 public:
  void changeTo(std::string& newPwd) {
    // 如果text.capacity() >= newPwd.size()可能重用text的内存：
    text = newPwd;
  }

 private:
  std::string text;
};
}  // namespace test2

// 这种情况下，按值传递的开销包括了内存分配和内存销毁，
// 可能会比std::string的移动操作高出几个数量级。

// 有趣的是，如果旧密码短于新密码，在赋值过程中就不可避免要销毁、分配内存，
// 这种情况，按值传递跟按引用传递的效率是一样的。
// 因此，基于赋值的形参拷贝操作开销取决于具体的实参的值，
// 这种分析适用于在动态分配内存中存值的形参类型。
// 不是所有类型都满足，但是很多，包括std::string和std::vector都是是这样。

// 这种潜在的开销增加仅在传递左值实参时才有，因为内存分配和释放通常在真正的拷贝中。
// 对右值实参，移动几乎就足够了。

// 结论是，使用通过赋值拷贝一个形参进行按值传递的函数的额外开销，取决于传递的类型，
// 左值和右值的比例，这个类型是否需要动态分配内存，以及，如果需要分配内存的话，
// 赋值操作符的具体实现，还有赋值目标占的内存至少要跟赋值源占的内存一样大。
// 对于std::string来说，开销还取决于实现是否使用了小字符串优化（SSO参考Item29），
// 如果是，那么要赋值的值是否匹配SSO缓冲区。

// 所以，当形参通过赋值进行拷贝时，分析按值传递的开销是复杂的。
// 通常，最有效的经验就是在证明没问题之前假设有问题，
// 就是除非已证明按值传递会为你需要的形参类型产生可接受的执行效率，
// 否则使用重载或者通用引用的实现方式。

// 到此为止，对于需要运行尽可能快的软件来说，按值传递可能不是一个好策略，
// 因为避免即使开销很小的移动操作也非常重要。
// 此外，有时并不能清楚知道会发生多少次移动操作。
// 在Widget::addName例子中，按值传递仅多了一次移动操作，
// 但是如果Widget::addName调用了Widget::validateName，这个函数也是按值传递。
// 并假设validateName调用了第三个函数，也是按值传递，在调用链中，每个函数都使用传值，
// 因为只多了一次移动的开销，但是整个调用链总体就会产生无法忍受的开销，
// 通过引用传递，调用链不会增加这种开销。
}  // namespace n5

namespace n6 {
// 跟性能无关，但总是需要考虑的是，不像按引用传递，按值传递会受到切片问题的影响。
// 但是如果要设计一个函数，来处理这样的形参：基类或者任何其派生类，
// 肯定不想声明一个那个类型的传值形参，因为会切掉传入的任意派生类对象的派生类特征：
class Widget {};
class SpecialWidget : public Widget {};
void processWidget(Widget w) {}

void func() {
  SpecialWidget sw;
  processWidget(sw);
}

// 除了效率问题外，切片问题是C++98中默认按值传递名声不好的另一个原因。
// 有充分的理由来说明为什么学习C++编程的第一件事就是避免用户自定义类型进行按值传递。

// C++11没有从根本上改变C++98对于按值传递的智慧。
// 通常，按值传递仍然会带来希望避免的性能下降，而且按值传递会导致切片问题。
// C++11中新的功能是区分了左值和右值实参。
// 利用对于可拷贝类型的右值的移动语义，需要重载或者通用引用，尽管两者都有其缺陷。
// 对于特殊的场景，可拷贝且移动开销小的类型，传递给总是会拷贝他们的一个函数，
// 并且切片也不需要考虑，这时，按值传递就提供了一种简单的实现方式，
// 效率接近传递引用的函数，但是避免了传引用方案的缺点。
}  // namespace n6

// 要记住的东西：
// 1.对于可拷贝，移动开销低，而且无条件被拷贝的形参，
//   按值传递效率基本与按引用传递效率一致，而且易于实现，还生成更少的目标代码；
// 2.通过构造拷贝形参可能比通过赋值拷贝形参开销大的多；
// 3.按值传递会引起切片问题，所说不适合基类形参类型。

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
      n2::func();
      break;
    case 2:
      n3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}