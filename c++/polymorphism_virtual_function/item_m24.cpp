#include <iostream>

using namespace std;

// 理解虚拟函数、多继承、虚基类和RTTI所需的代价

// C++编译器们必须实现语言的每一个特性。
// 这些实现的细节当然是由编译器来决定的，
// 并且不同的编译器有不同的方法实现语言的特性。
// 在多数情况下，你不用关心这些事情。
// 然而有些特性的实现对对象大小和其成员函数执行速度有很大的影响，
// 所以对于这些特性有一个基本的了解，
// 知道编译器可能在背后做了些什么，就显得很重要。
// 这种特性中最重要的例子是虚拟函数。

// 当调用一个虚拟函数时，被执行的代码必须与调用函数的对象的动态类型相一致；
// 指向对象的指针或引用的类型是不重要的。
// 编译器如何能够高效地提供这种行为呢？
// 大多数编译器是使用virtual table和virtual table pointers。
// virtual table和virtual table pointers通常被分别地称为vtbl和vptr。
// 一个vtbl通常是一个函数指针数组，
// 一些编译器使用链表来代替数组，但是基本方法是一样的。
// 在程序中的每个类只要声明了虚函数或继承了虚函数，
// 它就有自己的vtbl，并且类中vtbl的项目是指向虚函数实现体的指针。
class C1 {
 public:
  C1();
  virtual ~C1();
  virtual void f1();
  virtual int f2(char c) const;
  virtual void f3(const string& s);
  void f4() const;
};
// 虚函数表：C1::~C1|C1::f1|C1::f2|C1::f3
// 非虚函数f4不在表中，而且C1的构造函数也不在。
// 非虚函数（包括构造函数）就象普通的C函数那样被实现，
// 在性能上没有特殊的考虑。
class C2 : public C1 {
 public:
  C2();                        // 非虚函数
  virtual ~C2();               // 重定义函数
  virtual void f1();           // 重定义函数
  virtual void f5(char* str);  // 新的虚函数
};
// 虚函数表：C2::~C2|C1::~C1|C2::f1|C1::f2|C1::f3|C2::f5

// 虚函数所需的第一个代价：
// 你必须为每个包含虚函数的类的virtual talbe留出空间。
// 类的vtbl的大小与类中声明的虚函数的数量成正比，
// 包括从基类继承的虚函数。
// 每个类应该只有一个virtual table，
// 所以virtual table所需的空间不会太大，
// 但是如果你有大量的类或者在每个类中有大量的虚函数，
// 你会发现vtbl会占用大量的地址空间。

// 因为在程序里每个类只需要一个vtbl拷贝，
// 所以编译器肯定会遇到一个棘手的问题：把它放在哪里。
// 大多数程序和程序库由多个object（目标）文件连接而成，
// 但是每个object文件之间是独立的。
// 哪个object文件应该包含给定类的vtbl呢？
// 你可能会认为放在包含main函数的object文件里，但是程序库没有main，
// 而且无论如何包含main的源文件不会涉及很多需要vtbl的类。
// 编译器如何知道它们被要求建立那一个vtbl呢？
// 必须采取一种不同的方法，编译器厂商为此分成两个阵营。
// 对于提供集成开发环境（包含编译程序和连接程序）的厂商，
// 一种干脆的方法是为每一个可能需要vtbl的object文件生成一个vtbl拷贝。
// 连接程序然后去除重复的拷贝，
// 在最后的可执行文件或程序库里就为每个vtbl保留一个实例。
// 更普通的设计方法是采用启发式算法来决定哪一个object文件应该包含类的vtbl。
// 通常启发式算法是这样的：
// 要在一个object文件中生成一个类的vtbl，
// 要求该object文件包含该类的第一个非内联、
// 非纯虚拟函数定义（也就是类的实现体）。
// 因此上述C1类的vtbl将被放置到包含C1::~C1定义的object文件里，
// C2类的vtbl被放置到包含C1::~C2定义的object文件里。
// 实际当中，这种启发式算法效果很好。
// 但是如果你过分喜欢声明虚函数为内联函数，
// 如果在类中的所有虚函数都内声明为内联函数，启发式算法就会失败，
// 大多数基于启发式算法的编译器会在每个使用它的object文件中生成一个类的vtbl。
// 在大型系统里，这会导致程序包含同一个类的成百上千个vtbl拷贝！
// 大多数遵循这种启发式算法的编译器会给你一些方法来人工控制vtbl的生成，
// 但是一种更好的解决此问题的方法是避免把虚函数声明为内联函数。
// 有一些原因导致现在的编译器一般总是忽略虚函数的的inline指令。

// 虚函数所需的第二个代价是：
// 在每个包含虚函数的类的对象里，你必须为额外的指针付出代价。
// virtual table只实现了虚拟函数的一半机制，如果只有这些是没有用的。
// 只有用某种方法指出每个对象对应的vtbl时，
// 它们才能使用，这是vptr的工作，它来建立这种联系。
// 每个声明了虚函数的对象都带有它，
// 它是一个看不见的数据成员，指向对应类的virtual table。
// 这个看不见的数据成员也称为vptr，被编译器加在对象里，位置只有才编译器知道。
// 不同的编译器放置它的位置也不同，存在继承的情况下，
// 一个对象的vptr经常被数据成员所包围。
// 如果对象很小，这是一个很大的代价，比如如果你的对象平均只有4比特的成员数据，
// 那么额外的vptr会使成员数据大小增加一倍（假设vptr大小为4比特）。
// 在内存受到限制的系统里，这意味着你必须减少建立对象的数量。

void makeACall(C1* pC1) { pC1->f1(); }
// 仅仅看这段代码，不会知道调用的是哪个f1函数，C1::f1或C2::f1，
// 因为pC1可以指向C1对象也可以指向C2对象。
// 尽管如此编译器仍然得为在makeACall的f1函数的调用生成代码，
// 它必须确保无论pC1指向什么对象，函数的调用必须正确。
// 编译器生成的代码会做如下这些事情：
// 1.通过对象的vptr找到类的vtbl。
//   这是一个简单的操作，因为编译器知道在对象内哪里能找到vptr，
//   毕竟是由编译器放置的它们。
//   因此这个代价只是：
//   一个偏移调整（以得到vptr）；
//   一个指针的间接寻址（以得到vtbl）。
// 2.找到对应vtbl内的指向被调用函数的指针。
//   这也是很简单的，因为编译器为每个虚函数在vtbl内分配了一个唯一的索引。
//   这步的代价只是在vtbl数组内的一个偏移。
// 3.调用第二步找到的的指针所指向的函数。
//   类似(*pC1->vptr[i])(pC1);
//   调用被vtbl中第i个单元指向的函数，而pC1->vptr指向的是vtbl；
//   pC1被做为this指针传递给函数。
//   这几乎与调用非虚函数效率一样，
//   在大多数计算机上它多执行了很少的一些指令。
//   调用虚函数所需的代价基本上与通过函数指针调用函数一样，
//   虚函数本身通常不是性能的瓶颈。

// 虚函数所需的第三个代价：你实际上放弃了使用内联函数。
// 当通过对象调用虚函数时，它可以被内联，
// 但是大多数虚函数是通过对象的指针或引用被调用的，这种调用不能被内联。
// 因为这种调用是标准的调用方式，所以虚函数实际上不能被内联。
// 在实际运行中，虚函数所需的代价与内联函数有关。
// 实际上虚函数不能是内联的，
// 这是因为内联是指在编译期间用被调用的函数体本身来代替函数调用的指令，
// 但是虚函数的虚是指直到运行时才能知道要调用的是哪一个函数。
// 如果编译器在某个函数的调用点不知道具体是哪个函数被调用，
// 你就能知道为什么它不会内联该函数的调用。

// 在多继承里，在对象里为寻找vptr而进行的偏移量计算会变得更复杂。
// 在单个对象里有多个vptr（每一个基类对应一个）；
// 除了我们已经讨论过的单独的自己的vtbl以外，还得为基类生成特殊的vtbl。
// 因此增加了每个类和每个对象中的虚函数额外占用的空间，
// 而且运行时调用所需的代价也增加了一些。
// 多继承经常导致对虚基类的需求。
// 没有虚基类，如果一个派生类有一个以上从基类的继承路径，
// 基类的数据成员被复制到每一个继承类对象里，
// 继承类与基类间的每条路径都有一个拷贝。
// 程序员一般不会希望发生这种复制，而把基类定义为虚基类则可以消除这种复制。
// 然而虚基类本身会引起它们自己的代价，
// 因为虚基类的实现经常使用指向虚基类的指针做为避免复制的手段，
// 一个或者更多的指针被存储在对象里。
// 恐怖的多继承菱形：
class A {};
class B : virtual A {};
class C : virtual A {};
class D : public B, public C {};
// 一些编译器（特别是比较老的编译器），D对象会产生这样布局：
//  B的数据成员
//  指向虚基类的指针（最下面的A的数据成员）
//  C的数据成员
//  指向虚基类的指针（最下面的A的数据成员）
//  D的数据成员
//  A的数据成员
// 把基类的数据成员放在对象的最底端，这显得有些奇怪，但是它经常这么做。
// 当然如何实现是编译器的自由，它们想怎么做都可以。
// 如果在上述继承体系里的基类A有任何虚函数，对象D的内存布局就是这样的：
//  B的数据成员
//  vptr
//  指向虚基类的指针（最下面的A的数据成员）
//  C的数据成员
//  vptr
//  指向虚基类的指针（最下面的A的数据成员）
//  D的数据成员
//  A的数据成员
//  vptr
// 虽然存在四个类，但是只有三个vptr。
// 只要编译器喜欢，当然可以生成四个vptr，
// 但是三个已经足够了（它发现B和D能够共享一个vptr），
// 大多数编译器会利用这个机会来减少编译器生成的额外负担。
// 虚函数能使对象变得更大，而且不能使用内联；
// 继承和虚基类也会增加对象的大小。

// 运行时类型识别（RTTI）能让我们在运行时找到对象和类的有关信息，
// 所以肯定有某个地方存储了这些信息让我们查询。
// 这些信息被存储在类型为type_info的对象里，
// 你能通过使用typeid操作符访问一个类的type_info对象。
// 在每个类中仅仅需要一个RTTI的拷贝，但是必须有办法得到任何对象的类型信息。
// 我们保证可以获得一个对象动态类型信息，如果该类型有至少一个虚函数。
// 这使得RTTI数据似乎有些象virtual function talbe(虚函数表)。
// 每个类我们只需要信息的一个拷贝，
// 我们需要一种方法从任何包含虚函数的对象里获得合适的信息。
// 这种RTTI和virtual function table之间的相似点并不是巧合：
// RTTI被设计为在类的vtbl基础上实现。
// 例如，vtbl数组的索引0处可以包含一个type_info对象的指针，
// 这个对象属于该vtbl相对应的类。
// 使用这种实现方法，RTTI耗费的空间是：
// 在每个类的vtbl中的占用的额外单元再加上存储type_info对象的空间。
// 就象在多数程序里virtual table所占的内存空间并不值得注意一样，
// 你也不太可能因为type_info对象大小而遇到问题。

// 是对虚函数、多继承、虚基类以及RTTI所需主要代价的总结：
//  特性        增大对象尺寸      增加class数据        不能内联
//  虚函数          yes              yes              yes
//  多继承          yes              yes              no
//  虚基类          often         sometimes           no
//  RTTI           no               yes              no
// 一些人看到这个表格以后，会很吃惊，他们宣布我还是应该使用C。
// 很好，但是请记住如果没有这些特性所提供的功能，你必须手工编码来实现。
// 在多数情况下，你的人工模拟可能比编译器生成的代码效率更低，稳定性更差。
// 例如使用嵌套的switch语句或层叠的if－then－else语句模拟虚函数的调用，
// 其产生的代码比虚函数的调用还要多，而且代码运行速度也更慢。
// 再有，你必须自己人工跟踪对象类型，
// 这意味着对象会携带它们自己的类型标签（type tag）。
// 因此你不会得到更小的对象。

// 理解虚函数、多继承、虚基类、RTTI所需的代价是重要的，
// 但是如果你需要这些功能，
// 不管采取什么样的方法你都得为此付出代价，理解这点也同样重要。
// 有时你确实有一些合理的原因要绕过编译器生成的服务。
// 例如隐藏的vptr和指向虚基类的指针会使得，
// 在数据库中存储C++对象或跨进程移动它们变得困难，
// 所以你可能希望用某种方法模拟这些特性，能更加容易地完成这些任务。
// 不过从效率的观点来看，你自己编写代码不可能做得比编译器生成的代码更好。

int main() { return 0; }