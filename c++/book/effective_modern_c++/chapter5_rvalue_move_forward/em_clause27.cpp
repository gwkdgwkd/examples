#include <iostream>
#include <vector>

// 条款27：熟悉与万能引用类型进行重载的替代方案

// 条款26说过，万能引用类型进行重载会导致形形色色的问题，构造函数尤其问题严重。

namespace n1 {
// 舍弃重载

// 条款26的例子1，logAndAdd可以作为很多函数的代表，把打算进行重载的版本重新命名，
// 就可以避免依万能引用类型进行重载，以logAndAdd两个重载版本为例，
// 就可以分别改成logAndAddName和logAndAddNameIdx。
// 但这种方法不适用于例子2，Person类的构造函数，因为构造函数的名字是由语言固化的。
// 还有，彻底放弃重载也不是长久之计。
}  // namespace n1

namespace n2 {
// 传递const T&类型的形参

// 一种替代方法是回归C++98，使用传递左值常量引用类型来代替传递万能引用类型。
// 其实，这就是条款26做的第一种问题解决尝试，这种方法的缺点是达不到想要的高效率。
// 不过，在已知依万能引用类型进行重载会带来的不良效应以后，
// 放弃部分效率来保持简洁性不失为仍有一定吸引力的权衡结果。
}  // namespace n2

namespace n3 {
// 传值

// 一种经常能够提升性能，却不用增加复杂性的方法，
// 就是把传递的形参从引用类型替换成值类型，尽管这是反直觉的。
// 这种设计遵守了条款41的建议，当知道肯定需要复制形参时，考虑按值传递对象。

struct Widget {
  Widget() {}
  Widget(const char*) {}
  Widget(const Widget&) {}
  Widget(Widget&&) {}
};
Widget WidgetFromIdx(int idx) { return Widget(); }

class Person {
 public:
  explicit Person(Widget w) : w_(std::move(w)) {
    std::cout << "Person(T n)" << std::endl;
  }

  explicit Person(int idx) : w_(WidgetFromIdx(idx)) {
    std::cout << "Person(int idx)" << std::endl;
  }

 private:
  Widget w_;
};

void func() {
  // 由于Widget并没有只接受单个整形形参的构造函数，所有int或者类int类型，
  // 例如std::size_t，short，long等，都会调用int类型的构造函数重载版本：
  short s = 22;
  Person p1(s);  // Person(int idx)

  std::size_t size = 22;
  Person p2(size);  // Person(int idx)

  long l = 22l;
  Person p3(l);  // Person(int idx)

  int i = 22;
  Person p4(i);  // Person(int idx)

  // 调用int版本的重载函数，不应该用NULL：
  Person p5(NULL);  // Person(int idx)

  // 所有std::string类型的实参，包括可以构造出std::string对象的字面量，
  // 都会被传递给接受std::string类型的那个构造函数：
  Person p6("nihao");  // Person(T n)

  Widget w("nihao");
  Person p7(w);  // Person(T n)

  Person p8(nullptr);  // Person(T n)
}
}  // namespace n3

struct Widget {
  Widget() { std::cout << "Widget()" << std::endl; }
  Widget(int) { std::cout << "Widget(int)" << std::endl; }
  Widget(const char*) { std::cout << "Widget(const char *)" << std::endl; }
  Widget(const Widget&) { std::cout << "Widget(const Widget &)" << std::endl; }
  Widget(Widget&&) { std::cout << "Widget(Widget &&)" << std::endl; }
};
Widget WidgetFromIdx(int idx) { return Widget(); }

namespace n4 {
// 标签分派

// 无论是传递左值还是常量，都不支持完美转发。
// 如果使用万能引用的动机就是为了实施完美转发，那就只能采用万能引用，别无他途。
// 如果想放弃重载，也不想放弃万能引用，那又该如何避免依万能引用类型进行重载呢？

// 其实解决之道并非那么难，重载函数在调用时的决议，会考察所有重载版本的形参，
// 以及调用传入的实参，然后选择全局最佳匹配的函数，这需将所有的形参/实参组合考虑在内。
// 一个万能引用形参通常会导致的后果是无论传什么都给出一个精确的匹配结果，
// 不过，如果万能引用仅是形参列表的一部分，该列表中还有其他非万能引用类型形参的话，
// 那么只要该非万能引用形参具备充分差的匹配能力，
// 则它就足以将这个带有万能引用形参的重载版本踢出局，这个想法就是标签分派手法的基础。

std::vector<Widget> ws;
template <typename T>
void fImpl(T&& w, std::false_type) {
  std::cout << "void fImpl(T&& w)" << std::endl;
  ws.emplace_back(std::forward<T>(w));
}

void fImpl(int idx, std::true_type) {
  std::cout << "void fImpl(int idx)" << std::endl;
  ws.emplace_back(WidgetFromIdx(idx));
}

namespace test1 {
template <typename T>
void f(T&& w) {
  // 多了一个参数，用来表示形参w的类型是否为整形：
  fImpl(std::forward<T>(w), std::is_integral<T>());  // 不够正确
}
void func() {
  ws.reserve(10);

  Widget ww;
  f(ww);
  // Widget()
  // void fImpl(T&& w)
  // Widget(const Widget &)

  f(Widget("PersePhone"));
  // Widget(const char *)
  // void fImpl(T&& w)
  // Widget(Widget &&)

  f("PattyDog");
  // void fImpl(T&& w)
  // Widget(const char *)

  f(22);
  // void fImpl(int idx)
  // Widget()
  // Widget(Widget &&)

  // 条款28，如果给万能引用w的实参是个左值int，则T就会被推导为int&，这不是个整形，
  // 因为引用类型都不是整形，这意味着std::is_intergral<T>在函数接受了任意实参时，
  // 会得到结果假，尽管这样的实参确实表示了一个整型值：
  int i = 22;
  f(i);
  // void fImpl(T&& w)
  // Widget(int)
}
}  // namespace test1

namespace test2 {
template <typename T>
void f(T&& name) {
  // f会向fImpl传递一个布尔值，用以表示传递f的实参是否为整形，不过，
  // true和false都是运行期值，可是需要利用译期的重载决议来选择正确的fImpl重载版本。
  // 这就意味着需要一个对应于true的类型，和一个对应于false的不同类型。
  // C++标准库提供了名为std::true_type和std::false_type来满足这个普遍的需求。
  // 若T是整形，则经由f传递给fImpl的实参就会是个继承自std::true_type的对象。
  // 反之，若T不是整形，该实参就会是个继承自std::false_type的对象。

  fImpl(std::forward<T>(name),
#if __cplusplus == 201103L
        std::is_integral<typename std::remove_reference<T>::type>());
#elif __cplusplus == 201402L
        std::is_integral<std::remove_reference_t<T>>());
#endif
}

void func() {
  ws.reserve(10);

  f(22);
  // void fImpl(int idx)
  // Widget()
  // Widget(Widget &&)

  int i = 22;
  f(i);
  // void fImpl(int idx)
  // Widget()
  // Widget(Widget &&)

  short s = 22;
  f(s);
  // void fImpl(int idx)
  // Widget()
  // Widget(Widget &&)

  std::size_t size = 22;
  f(size);
  // void fImpl(int idx)
  // Widget()
  // Widget(Widget &&)
}
}  // namespace test2

// 在上述设计中，类型std::false_type和std::true_type就是所谓标签，
// 运用它们的唯一目的在于强制重载决议按我们想要的方向推进。
// 值得注意的是，这些形参甚至没有名字，它们在运行期不起任何作用，实际上，
// 希望编译器能够识别出这些标签并未使用过，从而将它们从程序的执行镜像中优化掉，
// 某些编译器的确会这样做，至少有时会。
// 针对f内的重载实现函数发起的调用把工作分派到正确的重载版本的手法就是创建适当的标签对象。
// 这种设计因而得名：标签分派，它是模板元编程的标准构件，
// 所以越是深入地查看当下的C++库代码，也就是越会频繁地看到这种设计。

// 就目的而言，更重要的不在于标签分派的工作细节，
// 而是它让得以将万能引用和重载加以组合却不会引发条款26所描述的问题的能力。
// 分派函数f接受的是个不受限制的万能引用形参，但该函数并未重载。
// 实现函数logAndAddImpl则实施了重载，每个重载版本都接受一个万能引用形参，
// 但重载决议确并不仅对这个万能引用形参有依赖，还对标签有依赖，
// 而标签则又加以设计以保证可以命中匹配的函数不会超过一个，这样的设计的结果是，
// 只有标签值才决定了调用的是哪个重载版本，万能引用总是给出精确匹配的事实，也就无关紧要了。
}  // namespace n4

namespace n5 {
// 对接受万能引用的模板施加限制

// 标签分派能够发挥作用的关键在于，存在一个单版本（无重载版本的）函数作为客户端API。
// 该单版本函数会把待完成的工作分派到实现函数。
// 创建无重载的分派函数通常并不难，但条款26所关注的第二个问题，
// 即关于Person类的完美转发构造函数的那个问题，却是个例外。
// 编译器自行生成复制和移动构造函数，所以如果仅仅撰写构造函数，然后在其中运用标签分派，
// 那么有些针对构造函数调用就可能会由编译器生成的构造函数接手处理，从而绕过了标签分派系统。

// 实际上，真正的问题并不在于编译器生成的函数有时候会绕过标签分派设计，
// 而在于编译器生成的函数并不能保证一定会绕过标签分派设计。
// 当收到使用左值对象进行同类型对象的复制请求时，几乎总会期望调用到的复制构造函数。
// 但是，就如条款26所演示的那样，只要提供一个接受万能引用形参的构造函数，
// 就会导致复制非常量左值时会调用到万能引用构造函数（而非复制构造函数）。
// 同一条款也解释了，如果基类中声明了一个完美转发构造函数，
// 则派生类以传统方式实现其复制和移动构造函数时，总会调用到该构造函数，
// 尽管正确行为应该是调用到基类的复制和移动构造函数。

// 对于这样的情况，也就是接受了万能引用形参的重载函数比想要的程度更贪婪，
// 而却又未贪婪到能够独当一面成为单版本分派函数的程度，标签分派就不是想要寻找的好伙伴了。
// 需要的是其他技巧，它可以让你把含有万能引用部分的函数模板被允许采用的条件砍掉一部分。
// 朋友，需要的这门绝技名叫std::enable_if。

// std::enable_if可以强制编译器忽略特定的模板，这样的模板称为禁用的，默认地，
// 所有的模板都是启用的，可是，使用了std::enable_if的模板只会在满足条件时才会启用。
// 在讨论的情况下，仅在传递给完美转发构造函数的类型不是Person时才启用它。
// 如果传递的类型是Person，就会想要禁用完美转发构造函数（即，让编译器忽略它），
// 因为这么一来，就会由类的复制或移动构造函数来接受处理这次调用了，
// 而这才是在用一个Person类型的对象来初始化另一个Person类型的对象时应有的结果。

// 想要表示这种想法并非那么难，不过语法实在让人望而却步：
namespace test1 {
class Person {
 public:
  template <typename T, typename = typename std::enable_if<
                            !std::is_same<Person, T>::value>::type>
  explicit Person(T&& n) : w_(std::forward<T>(n)) {
    std::cout << "Person(T&& n)" << std::endl;
  }

  Person(const Person&) { std::cout << "Person(const Person &)" << std::endl; }

 private:
  Widget w_;
};
void func() {
  Person p("Nancy");
  // auto cloneOfP(p);  // 还是调用了构造函数，没有调用复制构造函数，编译错误

  // 使用左值初始化万能引用时，T的类型推导结果总是左值引用。
  // 万能引用构造函数中的T的类型会被推导成Person&，类型Person和Person&不相同。

  std::cout << std::boolalpha << std::is_same<int, int&>::value << std::endl;
  // false
}
}  // namespace test1

namespace test2 {
// 仅当T不是Person类型时才启用Person类中模板构造函数，意味着在检查T的时应该忽略：
// 1.它是否是个引用，为判定万能引用构造函数是否应该被启用，
//   类型Person、Person&和Person&&都应该与Person作相同处理；
// 2.它是否带有const和volatile饰词，对目前关注的目的而言，
//   const Person，volatile Person和const volatile
//   Person都应该与Person作相同处理。
// 这意味着在判定T是否与Person相同之间，需要移除T类型带有的所有引用，const和volatile饰词。
// 标准库以提供了std::decay，它移除了T的引用和cv饰词，即const或volatile饰词，
// std::decay也用于把数组和函数类型强制转型成指针类型。

class Person {
 public:
  template <typename T, typename = typename std::enable_if<!std::is_same<
                            Person, typename std::decay<T>::type>::value>::type>
  explicit Person(T&& n) : w_(std::forward<T>(n)) {
    std::cout << "Person(T&& n)" << std::endl;
  }

  Person(const Person&) { std::cout << "Person(const Person &)" << std::endl; }

 private:
  Widget w_;
};

void func() {
  Person p("Nancy");  // 调用了万能引用构造函数
  // Widget(const char *)
  // Person(T&& n)

  // 当用一个Person类型的对象来构造另一个Person类型的对象时，无论它是左值还是右值，
  // 带不带const或volatile饰词，将永远不会调用到接受万能引用的构造函数：
  auto cloneOfP(p);  // 调用了拷贝构造函数
  // Widget()
  // Person(const Person &)

  std::cout.setf(std::ios_base::boolalpha);
  std::cout << std::is_same<int, std::decay<int&>::type>::value
            << std::endl;  // true
  std::cout << std::is_same<int, std::decay<int&&>::type>::value
            << std::endl;  // true
  std::cout << std::is_same<int, std::decay<const int>::type>::value
            << std::endl;  // true
  std::cout << std::is_same<int, std::decay<volatile int>::type>::value
            << std::endl;  // true
  std::cout << std::is_same<int, std::decay<const int&>::type>::value
            << std::endl;  // true
}

// 条款26的还有一个问题，当复制或者移动一个SpecialPerson类型的对象时，
// 会期望通过基类的复制或移动构造函数来完成该对象基类部分的复制或移动，
// 不过再这些函数中，传递给基类构造函数的是SpecialPerson类型的对象，
// 因为SpecialPerson和Person类型不同，std::decay的结果也是不同，
// 基类的万能引用构造函数会启用，后者精确匹配了SpecialPerson类型的实参后执行实例化。
// 所以，现有的代码在复制和移动它们的基类部分。
}  // namespace test2

namespace test3 {
// 因为派生类只是遵循着正常规则在实现其复制和移动构造函数，所以问题的解决还须在基类中完成。
// 更准确地说，就在那个决定了Person类的万能引用构造函数是否启用的条件中。
// 现在认识到，想要的并非是为与Person不同一的实参类型启用模板构造函数，
// 想要的是为与Person或继承自Person的类型都不同一的实参类型才启用模板构造，恼人的继承。

class Person {
 public:
  // 标准库中有个类型特征可以用来判定一个类型是否由另一个类型派生而来，std::is_base_of。
  // 若T2由T1派生而来，std::is_base_of<T1, T2>::value是真。
  // 这下就方便了，因为我们想要修改Person类的完美转发构造函数的启用条件，
  // 改成仅在类型T去除引用和cv饰词后，既非Person类型，亦非从Person派生的类型。
  // 以std::is_base_of代替std::is_same就可以得到想要的东西：
#if __cplusplus == 201103L
  template <typename T, typename = typename std::enable_if<!std::is_base_of<
                            Person, typename std::decay<T>::type>::value>::type>
#elif __cplusplus == 201402L
  template <typename T, typename = typename std::enable_if_t<
                            !std::is_base_of<Person, std::decay_t<T>>::value>>
#endif
  explicit Person(T&& n) : w_(std::forward<T>(n)) {
    std::cout << "Person(T&& n)" << std::endl;
  }

  Person(const Person&) { std::cout << "Person(const Person &)" << std::endl; }

 private:
  Widget w_;
};
class SpecialPerson : public Person {
 public:
  SpecialPerson() : Person(""){};
  SpecialPerson(const SpecialPerson& rhs) : Person(rhs) {}
  SpecialPerson(SpecialPerson&& rhs) : Person(std::move(rhs)) {}
};

void func() {
  Person p("Nancy");
  auto cloneOfP(p);
  // Widget(const char *)
  // Person(T&& n)
  // Widget()
  // Person(const Person &)

  SpecialPerson sp1;
  SpecialPerson sp2 = sp1;
  // Widget(const char *)
  // Person(T&& n)
  // Widget()
  // Person(const Person &)

  std::cout.setf(std::ios_base::boolalpha);
  std::cout << std::is_base_of<Person, Person>::value << std::endl;  // true
  std::cout << std::is_base_of<Person, SpecialPerson>::value << std::endl;
  // true
}
}  // namespace test3

namespace test4 {
class Person {
 public:
#if __cplusplus == 201103L
  template <typename T,
            typename = typename std::enable_if<
                !std::is_base_of<Person, typename std::decay<T>::type>::value &&
                !std::is_integral<std::remove_reference<T>>::value>::type>
#elif __cplusplus == 201402L
  // remove_reference怎么换成remove_reference_t？
  template <typename T, typename = typename std::enable_if_t<
                            !std::is_base_of<Person, std::decay_t<T>>::value &&
                            !std::is_integral<std::remove_reference<T>>::value>>
#endif
  explicit Person(T&& n) : w_(std::forward<T>(n)) {
    std::cout << "Person(T&& n)" << std::endl;
  }

  explicit Person(int idx) : w_(WidgetFromIdx(idx)) {
    std::cout << "Person(int idx)" << std::endl;
  }

  Person(const Person&) { std::cout << "Person(const Person &)" << std::endl; }

 private:
  Widget w_;
};

void func() {
  Person p1("Nancy");
  // Widget(const char *)
  // Person(T&& n)

  auto cloneOfP(p1);
  // Widget()
  // Person(const Person &)

  // 万能引用构造函数没有第二个判断条件的化，也是调用int类型的构造函数：
  Person p2(5);
  // Widget()
  // Person(int idx)

  int i = 5;
  Person p3(i);
  // Widget()
  // Person(int idx)

  short s = 5;
  Person p4(s);
  // Widget(int)
  // Person(T&& n)
}
}  // namespace test4

// 首先是利用了完美转发，达成了最高效率，其次又控制了万能引用和重载的组合，而非简单地禁用。
// 该技术可以实施于重载无法避免的场合，如构造函数。
}  // namespace n5

namespace n6 {
// 权衡

// 本条款关注的头三种技术，舍弃重载，传递const T&类型的形参和传值，
// 都需要对待调用的函数形参逐一指定类型，
// 而后两种技术，标签分派和对模板的限制启用，则利用了完美转发，因此无需指定形参类型。

// 完美转发效率更高，因为它出于和形参声明时的类型严格保持一致的目的，会避免创建临时对象。
// 在Person的构造函数中，完美转发允许把字符串字面量转发给某个接受Widget的构造函数。
// 而未使用完美转发的技术则一定先得从字符串字面量出发创建一个临时Widget对象，
// 方能满足Person类的构造函数的形参规格。

// 但是完美转发亦有不足，首先是针对某些类型无法实施完美转发，
// 尽管它们可以被传递到接受特定类型的函数，条款30探索了这些完美转发的失效案例。

// 其次是客户端传递了非法形参时，错误信息的可理解性，例如，假设在创建Person的对象时，
// 传递的是个chat16_t类型而非char类型的字符组成的字符串：
// Person p(u"onrad Zuse");
// 如果是本条款介绍的前三种技术，编译器会发现，可用的构造函数只能接受int或std::string。
// 所以，它们会产生出多少算是直截了当的错误信息来解释：
// 无法将const char16_t[12]强制转型到int或std::string。
// 而如果使用的是基于完美转发的技术时，情况大不一样。
// const char16_t类型的数组在绑定到构造函数的形参时，会一声不吭。
// 接着，它又被转发到Person的std::string类型的成员变量的构造函数中。
// 唯有在那里，调用的传递之物与所要求的形参之间的不匹配才会被发现。
// 如此产生的结果错误信息，会很多。

// 系统越是复杂，就越有可能是某个万能引用要经由数层函数调用完成转发，
// 才会抵达决定实参类型是否可接受的场所，万能引用转发的次数越多，
// 某些地方出错时给出的错误信息就越让人摸不着头脑，许多程序员都发现，
// 即使性能是首要的关注因素，在接口中也不去使用万能引用形参，根本原因就在于这个问题。

// 了解转发函数的万能引用形参应该用作Widget类型的对象的初始化类型，
// 所以可以使用static_assert来验证其能够扮演这个角色。
// std::is_constructible这个类型特征能够在编译期间判定具备某个类型的对象，
// 是否从另一个类型（或另一组类型）的对象（或另一组对象）出发完成构造：

class Person {
 public:
  template <typename T,
            typename = typename std::enable_if<
                !std::is_base_of<Person, typename std::decay<T>::type>::value &&
                !std::is_integral<std::remove_reference<T>>::value>::type>
  explicit Person(T&& n) : w_(std::forward<T>(n)) {
    std::cout << "Person(T&& n)" << std::endl;
    // 断言可以从T类型的对象构造一个Widget类型的对象：
    static_assert(std::is_constructible<Widget, T>::value,
                  "Parameter n cant used to construct a Widget");
    // 这会在客户端代码尝试从无法构造出Widget类型的对象的类型出发来创建Person类型的对象时，
    // 产生出该指定错误信息，不幸的是，static_assert位于构造函数的函数体内，
    // 而转发代码成员属于成员初始化列表的一部分，位于它之前，有用的错误信息会在后面。
  }
  explicit Person(int idx) : w_(WidgetFromIdx(idx)) {
    std::cout << "Person(int idx)" << std::endl;
  }
  Person(const Person&) { std::cout << "Person(const Person &)" << std::endl; }

 private:
  Widget w_;
};

struct S {};
void func() {
  S s;
  // Person p(s);
  // error: static assertion failed: Parameter n cant used to construct a Widget
}
}  // namespace n6

// 要记住的东西：
// 1.如果不使用万能引用和重载的组合，则替代方案包括使用：
//   a.彼此不同的函数名字；
//   b.传递const T&类型的形参；
//   c.传值；
//   d.标签分派。
// 2.经由std::enable_if对模板施加限制，就可以将万能引用和重载一起使用，
//   不过这种技术控制了编译器可以调用到接受万能引用的重载版本的条件；
// 3.万能引用形参通常在性能方面具备优势，但在易用性方面一般会有劣势。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 6]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n3::func();
      break;
    case 1:
      n4::test1::func();
      break;
    case 2:
      n4::test2::func();
      break;
    case 3:
      n5::test1::func();
      break;
    case 4:
      n5::test2::func();
      break;
    case 5:
      n5::test3::func();
      break;
    case 6:
      n5::test4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}