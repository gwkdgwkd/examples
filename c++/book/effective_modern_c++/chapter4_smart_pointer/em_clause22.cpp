#include <iostream>
#include <memory>
#include <vector>

// 条款22：当使用Pimpl惯用法，请在实现文件中定义特殊成员函数

// Pimpl（pointer to implementation）惯用法是指，
// 将类数据成员替换成一个指向包含具体实现的类（或结构体）的指针，
// 并将放在主类的数据成员们移动到实现类去，而这些数据成员的访问将通过指针间接访问。

#if __cplusplus == 201103L
template <typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts&&... params) {
  return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}
#elif __cplusplus == 201402L
using std::make_unique;
#endif

struct Gadget {};  // gadget.h文件中

namespace n1 {
class Widget {  // 定义在头文件widget.h
 public:
  Widget();

 private:
  std::string name;
  std::vector<double> data;
  Gadget g1, g2, g3;  // Gadget是用户自定义的类型
};

// 因为类Widget的数据成员包含有类型std::string，std::vector和Gadget，
// 定义有这些类型的头文件在类Widget编译的时候，必须被包含进来，
// 这意味着类Widget的使用者必须要#include<string>，<vector>以及gadget.h。
// 这些头文件将会增加类Widget使用者的编译时间，并且让这些使用者依赖于这些头文件。
// 如果一个头文件的内容变了，类Widget使用者也必须要重新编译。
// 标准库文件<string>和<vector>不是很常变，但是gadget.h可能会经常修订。
}  // namespace n1

namespace n2 {
// 在C++98中使用Pimpl惯用法，可以把Widget的数据成员替换成一个原始指针，
// 指向一个已经被声明过却还未被定义的结构体：
class Widget {  // 仍然在widget.h中
 public:
  Widget();
  ~Widget();

 private:
  struct Impl;  // 声明一个实现结构体以及指向它的指针
  Impl* pImpl;
};
// 因为类Widget不再提到类型std::string，std::vector以及Gadget，
// Widget的使用者不再需要为了这些类型而引入头文件。
// 这可以加速编译，并且意味着，如果这些头文件中有所变动，Widget的使用者不会受到影响。

// 一个已经被声明，却还未被实现的类型，被称为未完成类型，Widget::Impl就是这种类型。
// 能对一个未完成类型做的事很少，但是声明指向它的指针是可以的，Pimpl惯用法利用了这一点。

// Pimpl惯用法的第一步，是声明一个数据成员，它是个指针，指向一个未完成类型。
// 第二步是动态分配和回收一个对象，该对象包含那些以前在原来的类中的数据成员。
// 内存分配和回收的代码都写在实现文件里，比如，对于类Widget而言，写在Widget.cpp里：
#include <string>
#include <vector>
// #include "gadget.h"
// #include "widget.h"
struct Widget::Impl {
  std::string name;
  std::vector<double> data;
  Gadget g1, g2, g3;
};
Widget::Widget() : pImpl(new Impl) {}
Widget::~Widget() { delete pImpl; }
// 对于std::string，std::vector和Gadget的头文件的整体依赖依然存在。
// 然而，这些依赖从头文件widget.h移动到了widget.cpp，
// 在widget.h中时，它被所有Widget类的使用者包含，并且对他们可见，
// 在widget.cpp中时，该文件只被Widget类的实现者包含，并只对他可见。
}  // namespace n2

namespace n3 {
// C++98的代码使用了原始指针，原始的new和原始的delete，一切都让它如此的原始。
// 如果想要的只是在类Widget的构造函数动态分配Widget::impl对象，
// 在Widget对象销毁时一并销毁它，std::unique_ptr是最合适的工具。
// 在头文件中用std::unique_ptr替代原始指针，就有了头文件中如下代码：

class Widget {  // 在widget.h中
 public:
  Widget();

 private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;  // 使用智能指针而不是原始指针
};

// 实现文件widget.cpp也可以改成如下：
// #include "widget.h"
// #include "gadget.h"
#include <string>
#include <vector>
struct Widget::Impl {
  std::string name;
  std::vector<double> data;
  Gadget g1, g2, g3;
};
Widget::Widget() : pImpl(make_unique<Impl>()) {}
// unique_ptr在自身析构时，会自动销毁它所指向的对象，所以自己无需手动销毁任何东西，
// 也就不需要定义析构函数了，这就是智能指针的众多优点之一：从手动资源释放中解放出来。

// 以上代码能编译，但是，最普通的Widget用法却会导致编译出错：
// #include "widget.h"
// Widget w;
// 一般会提到把sizeof或delete应用到未完成类型上，对于未完成类型，使用以上操作是禁止的。

// 在Pimpl惯用法中使用std::unique_ptr会抛出错误，有点惊悚，因为：
// 1.std::unique_ptr宣称它支持未完成类型；
// 2.Pimpl惯用法是std::unique_ptr的最常见的使用情况之一。
// 幸运的是，让这段代码能正常运行很简单，只需要对出现的问题的原因有个基础的认识就可以了。

// 在对象w被析构时，例如离开了作用域，问题出现了，在这个时候，它的析构函数被调用。
// 在类的定义里使用了unique_ptr，所以没有声明一个析构函数，因为并没有任何代码需要写在里面。
// 根据编译器自动生成的特殊成员函数的规则，编译器会自动为我们生成一个析构函数。
// 在这个析构函数里，编译器会插入一些代码来调用类Widget的数据成员pImpl的析构函数。
// pImpl是一个unique_ptr<Widget::Impl>，也就是说，一个使用默认删除器的unique_ptr。
// 默认删除器是一个函数，它使用delete来销毁内置于unique_ptr的原始指针，在使用delete之前，
// 通常会使默认删除器使用C++11的static_assert来确保原始指针指向的类型不是一个未完成类型。
// 当编译器为Widget w的析构生成代码时，static_assert检查并且失败，
// 这通常是错误信息的来源，这些错误信息只在对象w销毁的地方出现，因为类Widget的析构函数，
// 正如其他的编译器生成的特殊成员函数一样，是暗含inline属性的。
// 错误信息自身往往指向对象w被创建的那行，因为这行代码明确地构造了这个对象，导致了后面的析构。
}  // namespace n3

namespace n4 {
// 为了解决这个问题，只需要确保在编译器生成销毁std::unique_ptr<Widget::Impl>的代码之前，
// Widget::Impl已经是一个完成类型，当编译器看到它的定义的时候，该类型就成为完成类型了。
// 但是Widget::Impl的定义在widget.cpp里。
// 成功编译的关键，就是在widget.cpp文件内，让编译器在看到Widget的析构函数实现之前，
// 也即编译器插入的，用来销毁unique_ptr这个数据成员的代码的那个位置，先定义Widget::Impl。

// 做出这样的调整很容易，只需要先在widget.h里，只声明类Widget的析构函数，但不要在这里定义它：
class Widget {
 public:
  Widget();
  ~Widget();

 private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};
// widget.cpp文件中，在结构体Widget::Impl被定义之后，再定义析构函数：
// #include "widget.h"
// #include "gadget.h"
#include <string>
#include <vector>
struct Widget::Impl {
  std::string name;
  std::vector<double> data;
  Gadget g1, g2, g3;
};
Widget::Widget() : pImpl(make_unique<Impl>()) {}
Widget::~Widget() {}

// 这样就可以了，增加的代码也最少，声明Widget析构函数只是为了在widget.cpp写出它的定义，
// 但是如果想强调编译器自动生成的析构函数会做和你一样正确的事情，
// 可以直接使用=default定义析构函数体：
// Widget::~Widget() = default;  // 代码效果一致
}  // namespace n4

namespace n5 {
// 使用了Pimpl惯用法的类自然适合支持移动操作，因为编译器自动生成的移动操作正合所意：
// 对其中的std::unique_ptr进行移动。
// 正如条款17所解释的那样，声明一个类Widget的析构函数会阻止编译器生成移动操作，
// 所以如果你想要支持移动操作，必须自己声明相关的函数。
// 考虑到编译器自动生成的版本会正常运行，可能会很想按如下方式实现它们：

namespace test1 {
class Widget {
 public:
  Widget();
  ~Widget();

  Widget(Widget&& rhs) = default;  // 思路正确，但代码错误
  Widget& operator=(Widget&& rhs) = default;

 private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};
// 会导致同样的错误，和之前的声明一个不带析构函数的类的错误一样，并且是因为同样的原因。
// 编译器生成的移动赋值操作符，在重新赋值之前，需要先销毁指针pImpl指向的对象。
// 然而在Widget的头文件里，pImpl指针指向的是一个未完成类型。
// 移动构造函数有所不同，移动构造函数的问题是编译器自动生成的代码里，包含有抛出异常的事件，
// 在这个事件里会生成销毁pImpl的代码，然而，销毁pImpl需要Impl是一个完成类型。
}  // namespace test1

namespace test2 {
// 解决方案也一样，把移动操作的定义移动到实现文件里：
class Widget {
  Widget();
  ~Widget();

  Widget(Widget&& rhs);
  Widget& operator=(Widget&& rhs);

 private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

#include <string>
struct Widget::Impl {};
Widget::Widget() : pImpl(make_unique<Impl>()) {}
Widget::~Widget() = default;
Widget::Widget(Widget&& rhs) = default;
Widget& Widget::operator=(Widget&& rhs) = default;
}  // namespace test2
}  // namespace n5

namespace n6 {
// Pimpl惯用法是用来减少类的实现和类使用者之间的编译依赖的一种方法，
// 但是，从概念而言，使用这种惯用法并不改变这个类的表现。
// 原来的类Widget包含有std::string，std::vector和Gadget数据成员，并且，
// 假设类型Gadget，如同std::string和std::vector一样，允许复制操作，
// 所以类Widget支持复制操作也很合理，必须要自己来写这些函数，因为：
// 1.对包含有只可移动类型，如std::unique_ptr的类，编译器不会生成复制操作；
// 2.即使编译器生成了，生成的复制操作也只会复制std::unique_ptr，浅拷贝，
//   而实际上需要复制指针所指向的对象，深拷贝。

// 在头文件里声明函数，而在实现文件里去实现他们：
class Widget {
 public:
  Widget(const Widget& rhs);
  Widget& operator=(const Widget& rhs);
  ~Widget();

 private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};
#include <string>
struct Widget::Impl {};
Widget::~Widget() = default;
Widget::Widget(const Widget& rhs) : pImpl(make_unique<Impl>(*rhs.pImpl)) {}
Widget& Widget::operator=(const Widget& rhs) {
  *pImpl = *rhs.pImpl;
  return *this;
}
}  // namespace n6

namespace n7 {
// 为了实现Pimpl惯用法，std::unique_ptr是使用的智能指针，
// 因为位于对象内部的pImpl指针，对所指向的对应实现的对象的享有独占所有权。
// 然而，有趣的是，如果使用std::shared_ptr而不是std::unique_ptr来做pImpl指针，
// 会发现本条款的建议不再适用，不需要在类Widget里声明析构函数，没有了用户定义析构函数，
// 编译器将会愉快地生成移动操作，并且将会如我们所期望般工作：
class Widget {
 public:
  Widget() {}

 private:
  struct Impl;
  std::shared_ptr<Impl> pImpl;  // 用std::shared_ptr
};

void func() {
  Widget w1;
  auto w2(std::move(w1));  // 移动构造w2
  w1 = std::move(w2);      // 移动赋值w1

  // 这些都能编译，w1将会被默认构造，它的值会被移动进w2，随后值将会被移动回w1，
  // 然后两者都会被销毁，因此导致指向的Widget::Impl对象一并也被销毁。
}

// std::unique_ptr和std::shared_ptr在pImpl指针上的表现上的区别的深层原因在于，
// 他们支持自定义删除器的方式不同，对std::unique_ptr而言，
// 删除器的类型是这个智能指针的一部分，这让编译器有可能生成更小的运行时数据结构和更快的运行代码。
// 这种更高效率的后果之一就是std::unique_ptr指向的类型，
// 在编译器的生成特殊成员函数（如析构函数，移动操作）被调用时，必须已经是一个完成类型。
// 而对std::shared_ptr而言，删除器的类型不是该智能指针的一部分，
// 这让它会生成更大的运行时数据结构和稍微慢点的代码，
// 但是当编译器生成的特殊成员函数被使用的时候，指向的对象不必是一个完成类型。
}  // namespace n7

// 对于Pimpl惯用法而言，在unique_ptr和shared_ptr的特性之间，没有一个比较好的折中。
// 因为对于像Widget的类以及像Widget::Impl的类之间的关系而言，
// 他们是独享占有权关系，这让std::unique_ptr使用起来很合适。
// 然而，有必要知道，在其他情况中，当共享所有权存在时，shared_ptr是很适用的选择的时候，
// 就没有std::unique_ptr所必需的声明、定义这样的麻烦事了。

// 要记住的东西：
// 1.Pimpl惯用法通过减少在类实现和类使用者之间的编译依赖来减少编译时间；
// 2.对于std::unique_ptr类型的pImpl指针，需要在头文件的类里声明特殊的成员函数，
//   但是在实现文件里面来实现他们，即使是编译器自动生成的代码可以工作，也要这么做；
// 3.以上的建议只适用于std::unique_ptr，不适用于std::shared_ptr。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}