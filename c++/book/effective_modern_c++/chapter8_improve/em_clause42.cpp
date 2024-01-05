#include <iostream>
#include <list>
#include <memory>
#include <regex>
#include <vector>

// 条款42：考虑使用置入代替插入

struct Widget {
  Widget() { std::cout << "Widget()" << std::endl; }
  Widget(const char*) { std::cout << "Widget(const char *)" << std::endl; }
  Widget(const Widget&) { std::cout << "Widget(const Widget &)" << std::endl; }
  Widget(Widget&&) { std::cout << "Widget(Widget &&)" << std::endl; }
  ~Widget() { std::cout << "~Widget()" << std::endl; }
};
std::vector<Widget> vw;

namespace n1 {
namespace test1 {
// 为什么push_back效率不高

// 引自C++11标准：
// template <class T, class Allocator = allocator<T>>
// class vector {
//  public:
//   ...
//   void push_back(const T& x);      // 插入左值
//   void push_back(T&& x);           // 插入右值
//   ...
// }

void func() {
  vw.reserve(10);

  vw.push_back("xyy");
  // Widget(const char *)
  // Widget(Widget &&)
  // ~Widget()
  // ~Widget()

  // 编译器会看到const char[4]与Widget的引用类型接受的形参性别之间的不匹配，
  // 而解决的办法就是通过生成代码以从字符串字面量触发创建Widget类型的临时变量，
  // 并将该临时对象传递给push_back，换言之，他们把这句调用看作下面这样：
  // ws.push_back(Widget("xyy"));

  // 这段代码顺利通过编译并且运行无误，所有人都开心地打卡下班了。
  // 所有人，除了那些性能狂人们，正是他们，认识到这段代码不像它应有的那样高效。

  // 为了在Widget类型的容器中创建一个新的元素，调用一次Widget的构造函数是应该的，
  // 但是上面的代码不会只发生一次构造函数调用，实际上，发生了两次。
  // 并且，还发生了一次Widget的析构函数调用。

  // 以下是对push_back的调用在运行期执行的全部动作：
  // 1.从字符串字面量xyy触发，创建Widget类型的临时对象，该对象没有名字，
  //   称之为temp，针对temp实施的构造，就是第一次的Widget构造函数的调用，
  //   因为是个临时对象，所以temp是个右值；
  // 2.temp被传递给push_back的右值重载版本，在那里它被绑定到右值引用形参x，
  //   然后，会在内存中为std::vector构造一个x的副本，
  //   这一次的构造（已经是第二次）结果就是在std::vector内创建了一个新的对象，
  //   用于将x复制到std::vector的构造函数，是移动构造函数，
  //   因为作为右值引用的x，在复制之前被转换成了右值；
  // 3.紧接着push_back返回的那一时刻，temp就被析构，所以，
  //   这就需要调用一次Widget的析构函数。
}
}  // namespace test1

namespace test2 {
// 对于n1中的问题，emplace_back可以解决：
// 它使用传入的任何实参在std::vector内构造一个Widget，不会涉及任何临时对象。
// emplace_back使用了完美转发，所以只要你没有遭遇完美转发的限制，
// 就可以通过emplace_back传递任意类型的任意数量和任意组合的实参。

void func() {
  vw.reserve(10);

  vw.emplace_back("xyy");
  // Widget(const char *)
  // ~Widget()
}
}  // namespace test2
}  // namespace n1

namespace n2 {
// emplace_back可用于任何支持push_back的标准容器。
// 相似的，所有支持push_front的标准容器也支持emplace_front。
// 还有，除了forward_list和array以外的所有标准容器，任何支持插入操作都支持置入操作。
// 关联容器提供了emplace_hint来补充它们带有hint迭代器的insert函数，
// 而std::forward_list也有着emplace_after与其insert_after一唱一和。
// 使得置入函数超越插入函数成为可能的，是置入函数更加灵活的接口。
// 插入函数接受的是待插入对象，而置入函数接受的则是待插入对象的构造函数实参。
// 这一区别就让置入函数得以避免临时对象的创建和析构，但插入函数就无法避免。

void func() {
  // 因为具备容器所持有之物的实参可以被传递给一个置入函数，而后，
  // 该实参会引发函数执行复制或移动构造，即使在插入函数并不要求创建临时对象的情况下，
  // 也可以使用置入函数，在那种情况下，插入函数和置入函数本质上做的是同一件事。

  Widget w("Donna Summer");
  // 下面两个调用语句都成立，并且对容器来说，净效果相同：
  vw.push_back(w);
  vw.emplace_back(w);
}

// 这么一来，置入函数就能做到插入函数所能做到的一切。
// 有时，他们可以比后者做的更高效一些，而且，至少在理论上不会比后者效率更低。
// 既然如此，何不总是使用置入函数呢？

// 因为，在有些情况下，正如预期的那样，置入的性能优于插入，但是，
// 不幸的是，还是存在插入函数运行得更快的情况，因为具体来说，取决于：
// 1.传递的实参类型；
// 2.使用的容器种类；
// 3.请求插入或置入的容器位置；
// 4.所持有类型构造函数的异常安全性；
// 5.还有，对于禁止出现重复的容器而言，容器中是否已经存在要添加的值，
//   std::set，std::map，std::unordered_set和std::unordered_map。

// 所以，这里适用一般的性能调优建议：欲确定置入或插入哪个运行更快，需对两者实施基准测试。
// 这样的说法当然不尽如人意，所以你肯定会高兴的得知，
// 有一种启发式思路可以帮助确定置入功能在哪些情况下极有可能值得一试。
// 如果下列情况都成立，那么置入将几乎肯定比插入更高效。

namespace test1 {
// 欲添加的值是以构造而非赋值方式加入容器。
// 本条款第一例，即从xyy出发创建std::string类型对象，并加入std::vector，
// 就能看出该值是被添加到了vs的结尾，该位置尚不存在对象。
// 是故，新值必须以构造方式加入std::vector。
// 若修订此例，使得新的std::string类型对象去到某个已被某对象占据的位置，
// 则完全变成另一回事了，考虑如下代码：
// vw.emplace_back(vw.begin(), "xyy");  // 向vw的开头添加xyy

// 对于这代码，很少会有实现是将待添加的std::string在由vs[0]占用的内存中实施构造。
// 这里一般会采用不同的手法，即移动赋值的方式来让该值就位，但既然是移动赋值，
// 总要有个作为源的移动对象，也就是意味着需要创建一个临时对象作为移动的源。
// 由于置入相对于插入的主要优点在于既不会创建也不会析构临时对象，
// 那么当添加的值是经由赋值放入容器的时候，置入的边际效用也就趋于消失了。

// 哎，向容器中添加值究竟是通过构造还是赋值，这一般取决于实现者。
// 但是，启发式思维会再一次派上用场。

// 基于节点的容器几乎总是使用构造来添加新值，而大多数标准容器都是基于节点的。
// 仅有的一些例外是std::vector，std::deque和std::string，
// std::array也不基于节点，但它根本不支持插入或置入，所以和这里的讨论不相干。
// 在非基于节点的容器中，可以可靠的说，emplace_back是使用构造函数非赋值来将新值就位的，
// 而这一点对于std::deque的emplace_front来说也一样成立。
}  // namespace test1

namespace test2 {
// 传递的实参类型与容器持有物的类型不同。
// 可以再次看出，置入相对于插入的优势通常源于这一个事实，
// 即当传递的实参类型并非容器持有之物的类型时，其接口不要求创建和析构临时对象。
// 当类型为T的对象被添加到container<T>中时，没有理由期望置入的运行速度会比插入快，
// 因为并不需要为了满足插入的接口去创建临时对象。
}

namespace test3 {
// 容器不太可能由于出现重复情况而拒绝待添加的新值。
// 这意味着，或者是容器允许重复值，或者所添加的大部分值都满足唯一性。
// 这个因素之所以值得一提，是因为，欲检测某值是否已经在容器中，
// 置入的实现通常会使用该新值创建一个节点，以便将该节点的值与容器的现有节点进行比较。
// 如果待添加的值尚不在容器中，则将节点链入该容器，但是，如果该节点已经存在，
// 则置入就会中止，节点也就会实施析构，这意味着其构造和析构的成本是被浪费掉了。
}

// vw.emplace_back("xyy");
// vw.emplace_back(50, 'x');
// 上面已列出的调用语句满足所有的判断准：
// 1.在容器尾部以构造新值；
// 2.传递和类型有异于容器持有之物的类型；
// 3.未使用拒绝重复值的容器。
// 则它们就比对应的push_back调用要运行的更快。
}  // namespace n2

namespace n3 {
// 在决定是否选用置入函数时，还有其他两个问题值得操心。

namespace test1 {
// 第一个和资源管理有关。
// 假定有个持有std::shared_ptr<Widget>类型对象的容器：
std::list<std::shared_ptr<Widget>> ptrs;

// 而想向其添加一个需要通过自定义删除器来释放的std::shared_ptr。
// 只要可行就应该使用std::make_shared来创建std::shared_ptr类型的对象，
// 但它同时也认可，在某些情况下无法这样做，而其中之一，就是需要指定自定义删除器的情况。
// 在此情况下，就必须直接使用new来获取裸指针以备std::shared_ptr加以管理。
void killWidget(Widget* pWidget) {}

void func() {
  ptrs.push_back(std::shared_ptr<Widget>(new Widget, killWidget));
  ptrs.push_back({new Widget, killWidget});  // 意义相同

  // 不管写成上面两者中的哪样，在调用push_back之前，
  // 都会创建一个std::shared_ptr类型的临时对象。
  // push_back的形参是个std::shared_ptr类型的引用，
  // 所以必须存在一个std::shared_ptr类型独享来让该形参指涉到。

  // 如果选用了emplace_back，本可以避免创建std::shared_ptr类型的临时独享，
  // 但是在本例的情况下，该临时对象带来的收益远超其成本。
  // 考虑下面这个有可能发生的事件序列：
  // 1.上述两个调用无论哪个都会构造一个std::shared_ptr<Widget>类型的临时对象，
  //   用以持有从new Widget返回的裸指针，该对象暂称为temp；
  // 2.push_back会按引用方式接受temp，
  //   在为链表节点分配内存以持有temp的副本的过程中，抛出了内存不足的异常；
  // 3.该异常传播到了push_back之外，temp被析构，作为给Widget兜底的，
  //   指向它并对其施加管理的std::shared_ptr<Widget>类型对象会自动释放该Widget，
  //   在本例的情况下，会调用killWidget达成该目的。

  // 即使发生异常，也没有资源泄露。
  // 在push_back的调用过程中，从new Widget出发构造的Widget，
  // 会在为管理它创建的std::shared_ptr(temp)的析构函数中得到释放，岁月静好。

  ptrs.emplace_back(new Widget, killWidget);
  // 如果调用的是emplace_back，而不是push_back，会发生什么：
  // 1.从new Widget返回的裸指针被完美转发，
  //   并运行到emplace_back内为链表节点分配内存的执行点，
  //   然而，该内存分配失败，并抛出了内存不足的异常；
  // 2.该异常传播到了emplace_back之外，作为唯一可以获取堆上Widget的抓手的裸指针，
  //   却丢失了，那个Widget连同他拥有的任何资源都发生了泄漏。

  // 在这种场景下，岁月不再静好，并且故障不能归罪于std::shared_ptr。
  // 即使换用std::unique_ptr的自定义删除器，同样的问题仍然可能会现身。
  // 从根本上讲，像std::shared_ptr和std::unique_ptr这样的资源管理类若要发挥作用，
  // 前提是资源比如从new出发的裸指针会立即传递给资源管理对象的构造函数。
  // std::make_shared和std::make_unique这样的函数会把这一点自动化，
  // 这个事实真是为何他们如此重要的原因之一。

  // 在调用持有资源管理对象的容器的插入函数时，函数的形参类型通常能确保在资源的获取，
  // 和对资源管理的对象实施构造之间不再有任何其他动作，而在置入函数中，
  // 完美转发会推迟资源管理对象的创建，直到他们能够在容器的内存中构造为止。
  // 这开了一个天窗，其中就会因异常而导致资源泄露，所有的标准容器对此都在劫难逃。
  // 在处理持有资源管理对象的容器时，必须小心确保在选用了置入而非插入函数时，
  // 不会在提升了一点代码效率的同时，却因异常安全性的削弱而赔的精光。

  // 绝不应该把new Widget传给emplace_back、push_back或者大多数其他函数，
  // 因为正如Item21所解释过的，这可能会导致刚才所讨论的异常安全问题，要把这扇门关闭，
  // 就需要从new Widget中获取指针并将其在独立语句中转交给其他资源管理对象，
  // 然后将该对象作为右值传递给你最初想要向其传递new Widget的函数，
  // Item21涵盖了该技术的更多细节，所以，选用了push_back的代码有应该这样写：
  std::shared_ptr<Widget> spw1(new Widget, killWidget);
  ptrs.push_back(std::move(spw1));
  std::shared_ptr<Widget> spw2(new Widget, killWidget);
  ptrs.emplace_back(std::move(spw2));

  // 无论选用哪个，这个途径都产生了构造和析构spw的成本。
  // 考虑选用置入而非插入的动机就在于避免容器所持有之物的类型的临时对象的成本，
  // 而这成本正是spw所体现的概念，当你向容器中添加的是资源管理对象，并遵循了正确的做法，
  // 以确保在资源的获取和将其移交给资源管理对象之间没有任何多余的动作的话，
  // 置入函数的性能表现就不太会在此情形下仍然超越插入函数。
}
}  // namespace test1

namespace test2 {
// 置入函数第二个值得一提的方面，是它们与带有explicit声明饰词的构造函数之间的互动。

std::vector<std::regex> regexes;
void func() {
  regexes.emplace_back(nullptr);

  // 在输入时没有注意到该错误，而编译器也不报错，最终浪费了大把时间来调试，
  // 找了半天，你终于发现自己在正则表达式容器中插入了一个空指针，但这怎么可能呢？
  // 指针本根不是正则表达式啊，而且如果试图这样做：
  // std::regex r1 = nullptr;

  // 如果选用的是push_back而非emplace_back，编译器同样也会报错：
  // regexes.push_back(nullptr);

  // 这种令人好奇的行为源自于std::regex对象可以从字符串触发来构造这一事实。
  // 这就使得下面这样有用的代码成为合法的：
  std::regex uppperCaseWord("[A-Z]+");
  // 从字符串出发来构造std::regex类型对象，肯定会导致相对较高昂的运行期成本，
  // 因此，为了尽可能地减少无意间招致这种开销的可能性，
  // 接受const char*指针的std::regex构造函数以explicit饰词声明。

  // 对于不能编译的语句，都要求了一次从指针到std::regex的隐式类型转换，
  // 而由于该构造函数带有explicit饰词声明，这样的类型转换被阻止了。

  // 然而，在emplace_back的调用过程中，却没有声称传递的是个std::regex对象。
  // 取而代之的是，我们向std::regex对象传递的是个构造函数实参。
  // 这不但不被视作隐式类型转换的请求，反而在编译器看来是等同于写了这样的代码：
  std::regex r2(nullptr);  // 虽然能通过编译，但却有着未定义行为
  // 接受const char*指针的std::regex构造函数要求：
  // 指向的字符串包含一个有意义的正则表达式，而空指针并不符合该要求。

  // 用标准的官方术语来说，用于采用等号初始化的语法对应于所谓的复制初始化。
  // 相对的，使用括号用于初始化的语法会产生所谓的直接初始化。
  // 复制初始化是不允许调用带有explicit声明饰词的构造函数的，但直接初始化就允许。
  // 是故，对r1实施初始化的那一行通不过编译，但对r2实施初始化的那一行就可以通过编译。

  // 回过头来再说push_back和emplace_back，或更一般地对插入函数和置入函数作对比。
  // 置入函数使用的是直接初始化，所以他们就能够调用带有explicit声明饰词的构造函数。
  // 而插入函数使用的是复制初始化，它们就不能调用带有explicit声明饰词的构造函数。

  // 这里得到的教训是，在使用置入函数时，要特别小心去保证传递了正确的实参，
  // 因为即使是带有explicit声明饰词的构造函数也会被编译器纳入考虑范围，
  // 因为它会尽力去找到某种方法来解释代码以使得它合法。
}
}  // namespace test2
}  // namespace n3

// 要记住的东西：
// 1.
// 2.
// 3.

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}