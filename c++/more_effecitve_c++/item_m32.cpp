#include <iostream>

using namespace std;

// 在未来时态下开发程序

// 作为软件开发人员，我们也许知道得不多，但我们知道万物都会变化。我们没必要知道什么将发生变化，这么变化又怎么发生，以什么时候发生，在哪里发生，但我们知道：万物都会变化。

// 好的软件能够适应变化。它提供新的特性，适应到新的平台，满足新的需求，处理新的输入。软件的灵活性、健壮性、可靠性不是来自于意外。
// 它是程序员们在满足了现在的需求并关注了将来的可能后设计和实现出来的。这样的软件（接受小改变的软件）是那些在未来时态下开发程序的人写出来的。

// 要在未来时态下开发程序，就必须接受事物会发生变化，并为此作了准备。这是应该考虑的：
//  新的函数将被加入到函数库中，新的重载将发生，于是要注意那些含糊的函数调用行为的结果；
//  新的类将会加入继承层次，现在的派生类将会是以后的基类，并已为此作好准备；
//  将会编制新的应用软件，函数将在新的运行环境下被调用，它们应该被写得在新平台上运行正确；
//  程序的维护人员通常不是原来编写它们的人，因此应该被设计得易于被别人理解、维护和扩充。

// 这么做的一种方法是：用C++语言自己来表达设计上的约束条件，而不是用注释或文档。
//  如果一个类被设计得不会被继承，不要只是在其头文件中加个注释，用C++的方法来阻止继承，Item M26显示了这个技巧；
//  如果一个类需要其实例全部创建在堆中，不要只是对用户说了这么一句，用Item M27的方法来强迫这一点。
//  如果拷贝构造和赋值对一个类是没有意义的，通过申明它们为私有来阻止这些操作（见Item E27）。
// C++提供了强大的功能、灵活度和表达力。用语言提供的这些特性来强迫程序符合设计。

// 应该判断一个函数的含意，以及它被派生类重定义的话是否有意义。如果是有意义的，申明它为虚，即使没有人立即重定义它。
// 如果不是的话，申明它为非虚，并且不要在以后为了便于某人而更改；确保更改是对整个类的运行环境和类所表示的抽象是有意义的。

// 处理每个类的赋值和拷贝构造函数，即使“从没人这样做过”。他们现在没有这么做并不意味着他们以后不这么做（见Item E18）。
// 如果这些函数是难以实现的，那么申明它们为私有。这样，不会有人误调编译器提供的默认版本而做错事。

// 基于最小惊讶法则：努力提供这样的类，它们的操作和函数有自然的语法和直观的语义。和内建数据类型的行为保持一致：拿不定主意时，仿照int来做。

// 要承认：只要是能被人做的，就有人这么做（WQ：莫菲法则）。
// 他们会抛异常；会用自己给自己赋值；在没有赋初值前就使用对象；给对象赋了值而没有使用；会赋过大的值、过小的值或空值。
// 一般而言，只要能编译通过，就有人会这么做。所以，要使得自己的类易于被正确使用而难以误用。要承认用户可能犯错误，所以要将你的类设计得可以防止、检测或修正这些错误。

// 努力于可移植的代码。写可移植的代码并不比不可移植的代码难太多，只有在性能极其重要时采用不可移植的结构才是可取的。
// 即使是为特定的硬件设计的程序也经常被移植，因为这些平台在几年内就会有一个数量级的性能提升。
// 可移植的代码使得你在更换平台是比较容易，扩大你的用户基础，吹嘘支持开放平台。这也使得你赌错了操作系统时比较容易补救。

// 将你的代码设计得当需要变化时，影响是局部的。尽可能地封装；将实现细节申明为私有。只要可能，使用无名的命名空间和文件内的静态对象或函数。
// 避免导致虚基类的设计，因为这种类需要每个派生类都直接初始化它－－即使是那些间接派生类。
// 避免需要RTTI的设计，它需要if.then.else型的瀑布结构。每次，类的继承层次变了，每组if.then.else语句都需要更新，如果你忘掉了一个，你不会从编译器得到任何告警。

// 这是著名的老生常谈般的告戒，但大部分程序员仍然违背它。看这条一个著名C++专家提出忠告（很不幸，许多作者也这么说）：
// 你需要虚析构函数，只要有人delete一个实际值向D的B*。
class B1 {
 public:
  ~B1() { cout << "~B1" << endl; }
};
class D1 : public B1 {
 public:
  ~D1() { cout << "~D1" << endl; }
};
class B2 {
 public:
  virtual ~B2() { cout << "~B2" << endl; }
};
class D2 : public B2 {
 public:
  ~D2() { cout << "~D2" << endl; }
};

// 如果一个公有基类没有虚析构函数，所有的派生类基其成员函数都不应该有析构函数。也就是说，这是没问题的：
class string1 {
 public:
  ~string1();
};
class B3 {};  // no data members with dtors,no virtual dtor needed
// 但从B继承一个新类后，事情就变了：
class D3 : public B3 {  // NOW ~B needs to be virtual
  string1 name;
};

// 如果多重继承体系有许多析构函数，每个基类都应该有应该虚析构函数。

// 未来时态的考虑完全不同。不是问一个类现在正被怎么使用，而是问这个类是被设计为怎么去使用的。
// 未来时态的考虑认为：如果一个类被设计为作一个基类使用（即使现在还没有被这么使用），它就应该有一个虚析构函数。
// 这样的类在现在和将来都行为正确，并且当新类从它们派生时并不影响其它库用户。
// 至少，它们没有任何影响，直到其析构函数被使用。如果需要对类的额外修改，其它用户将受影响。

// 有一个商业类库（在 C++标准运行库申明 string 以前）包含了一个没有虚析构函数的string类。其生产商解释：
// 我们没有使用虚析构函数，因为我们不想这个string类有vtbl。我们甚至不期望想有一个string*，所以这不成为问题。我们很清楚这么做将造成的困难。
// 这是进行时态的考虑还是未来时态的考虑？
// 当然，vbtl有个技术上的问题。大部分string类的实现都是对象内部只有一个char*指针，所以增加一个vptr造成每个string类的大小翻倍。
// 所以可以理解为什么不肯实现它，尤其是对于string这样频繁出现高密度使用的类。这样的类是属于影响程序性能的那20%的部分的。
// 还有，一个string对象的全部内存－它自己加上所指向的在堆中的字符串，通常远大于保存一个char*指针的大小。从这个方面来说，为vptr增加的花费并不是那么重要的。
// 不过，这仍然是个合法的设计。的确，ISO/ANSI标准委员会似乎是这么想的：标准string类型有一个非虚的析构函数。
// 更有问题的是生产商的注释：“我们甚至不期望有一个string*，所以这不成为问题”。
// 这可能是正确的，但他们的这个string类是提供给数以千记的开发人员使用的类库的一部分。有如此多的开发人员，每个人对C++的掌握程度都不同，每个人做事的方法也都不同。
// 这些人都了解string没有虚析构函数的后果吗？
// 生产商确信他们的客户知道没有虚析构函数时，用string*指针delete对象时可能工作不正确，在string的指针和引用上使用RTTI操作可能得到不正确的信息吗？
// 这个类是易于正确使用而不容易用错的吗？
// 这个生产商应该提供明确的文档以指出他的string类没有被设计得可被继承的，但如果程序员没注意到这个警告或未能读到这个文档时会发生什么？
// 一个可选方法是用C++自己来阻止继承。Item M26描述了怎么限制对象只生成于堆中，以及用auto_ptr对象来操作堆中的对象。
// 构造string对象的接口将不符合传统也不方便，需要这样：
// auto_ptr<String> ps(String::makeString("Future tense C++"));
// 来代替这个：
// String s("Future tense C++");
// 但，多半，为了减少不正确的继承行为是值得换用不方便的语法的。对string类，这未必很合算，但对其它类，这样的交换是完全值得的。

// 未来时态的考虑只是简单地增加了一些额外约束：
//  1 提供完备的类，即使某些部分现在还没有被使用。如果有了新的需求，你不用回过头去改它们。
//  2 将你的接口设计得便于常见操作并防止常见错误。使得类容易正确使用而不易用错。例如，阻止拷贝构造和赋值操作，如果它们对这个类没有意义的话。防止部分赋值。
//  3 如果没有限制你不能通用化你的代码，那么通用化它。例如，如果在写树的遍历算法，考虑将它通用得可以处理任何有向不循环图。
// 未来时态的考虑增加了你的代码的可重用性、可维护性、健壮性，已及在环境发生改变时易于修改。它必须与进行时态的约束条件进行取舍。
// 太多的程序员们只关注于现在的需要，然而这么做牺牲了其软件的长期生存能力。是与众不同的，是离经叛道的，在未来时态下开发程序。

int main() {
  B1 *pb = new D1;
  delete pb;
  // ~B1

  B2 *pb2 = new D2;
  delete pb2;
  // ~D2
  // ~B2

  return 0;
}