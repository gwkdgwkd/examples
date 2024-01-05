#include <iostream>
#include <vector>

// 条款6：当auto推导出非预期类型是应当使用显式的类型初始化

namespace n1 {
// 条款5解释了使用auto声明变量相对于显式指定类型而言，可以带来若干技术上的优势。
// 但是有的情况下，auto类型推导的结果与预期不符合。

// std::vector<bool>应该持有的是bool类型的元素，
// 但std::vector<bool>的operator[]返回值并不是容器中的元素的引用，
// 而是std::vector<bool>::reference类型的对象，定义在std::vector<bool>里面的类。
// 除bool外的其他所有形参类型，std::vector::operator[]都返回容器元素的引用。

// 之所以要弄出个std::vector<bool>::reference，是因为std::vector<bool>做过特化，
// 用了一种压缩形式表示其持有的bool元素，每个bool元素用一个比特来表示。
// 这种做法给std::vector<bool>的operator[]带来了一个问题，
// 因为按理说std::vector<bool>的operator[]应该返回一个T&，然而C++却禁止bit的引用，
// 既然不能返回bool&，std::vector<bool>的operator[]就返回一个表现的像bool&的对象，
// std::vector<bool>::reference类型的对象就要在所有能用bool&的地方保证它们也能用。
// 实现这个效果的原理是，std::vector<bool>::reference做了一个向bool&的隐式类型转换。

struct Widget {};
std::vector<bool> features(const Widget& w) {}
void processWidget(const Widget&, bool) {}
void func1() {
  Widget w;
  bool hightPriority = features(w)[5];
  processWidget(w, hightPriority);  // 按照w的优先级来处理

  // 上面的代码没有问题，运行正常

  // features返回了一个std::vector<bool>对象，然后针对该对象执行operator[]，
  // 而后，operator[]返回一个std::vector<bool>::reference类型的对象，
  // 该对象被隐式转换为一个初始化highPriority所需的bool对象。
}

void func2() {
  Widget w;
  auto hightPriority = features(w)[5];  // 换成auto
  processWidget(w, hightPriority);  // 行为未定义，highPriority含有悬空指针

  // 和前面一样,features返回了一个std::vector<bool>对象，然后针对该对象执行operator[]，
  // 然后，operator[]返回一个std::vector<bool>::reference类型的对象，从这里开始不一样，
  // auto会把highPriority的类型推导成std::vector<bool>::reference，这么一来，
  // highPriority的值就完全不可能会是features所返回的std::vector<bool>对象的第5个比特了，
  // highPriority的值取决于std::vector<bool>::reference的实现，有一种实现让对象含有指针，
  // 指向一个机器字，该机器字持有那个被引用的比特，再加上基于那个比特对应的字的偏移量。
  // 考虑一下，如果std::vector<bool>::reference真的是这样实现的话，highPrority如何初始化？
  // 对features的调用会返回一个std::vector<bool>的临时对象，该对象没有名字，暂称为temp，
  // 针对temp执行operator[]，返回一个std::vector<bool>::reference类型的对象，
  // 该对象含有一个指涉到机器字的指针，该机器字在一个持有temp所管理的那样比特的数据结构中，
  // 还要加上在第5位所对应的偏移量，由于highPriority是std::vector<bool>::reference的副本，
  // 所以highPriority也含有一个指向temp中的机器字的指针，加上还要加上在第5位所对应的偏移量。
  // 在表达式结束处，temp会被析构，因为它是一个临时对象，结果，highPriority会含有一个悬空指针，
  // 最终导致调用processWidget时出现的未定义行为。
}

void func() {
  func1();
  func2();
}
}  // namespace n1

namespace n2 {
// std::vector<bool>::reference是代理类的一个例子：
// 一个类的存在是为了模拟和对外行为和另外一个类保持一致，代理类在各种各样的目的上被使用。
// std::vector<bool>::reference是为了提供对std::vector<bool>的operator[]的错觉，
// 让它返回一个对bit的引用，而且标准库的智能指针也是一些对托管资源的代理类，
// 使得他们的资源管理类似于原始指针，代理类的功能是良好确定的，
// 事实上，代理模式是软件设计模式中最坚挺的成员之一。

// 一些代理类被设计用来隔离用户，这就是std::shared_ptr和std::unique_ptr的情况。
// 另外一些是为了或多或少的不可见性，std::vector<bool>::reference就是这样不可见的代理，
// 和他类似的是std::bitset，对应的是std::bitset::reference。

// 同时在一些C++库里面的类存在一种被称作表达式模板的技术，这些库刚开始是为了提高数值运算效率，
// 提供一个Matrix类和Matrix对象m1，m2，m3和m4，举一个例子，下面的表达式：
// Matrix sum = m1 + m2+ m3 + m4;
// 可以计算的更快如果Matrix的operator+返回一个结果的代理而不是结果本身，这是因为，
// 对于两个Matrix，operator+可能返回类似于Sum<Matrix,Matrix>的代理而不是一个Matrix对象。
// 和std::vector<bool>::reference一样，这里会有一个隐式的从代理类到Matrix的转换，
// 这个可能允许sum由=右边的表达式产生的代理对象进行初始化。
// 其中的对象可能会编码整个初始化表达式，也就是，
// 变成类似于Sum<Sum<Sum<Matrix,Matrix>,Matrix>,Matrix>的类型，这是客户端需要屏蔽的类型。

// 作为一个通用的法则，不可见的代理类不能和auto愉快的玩耍，
// 这种类常常它的生命周期不会被设计成超过单个的语句，所以创造这样的类型的变量是会违反库的设计假定。
// 这就是std::vector<bool>::reference的情况，而且可以看到这种违背约定的做法会导致未定义的行为。

// 因此要避免使用下面的代码形式：
// auto sameVar = expression of "invisible" proxy class type;

// 但是怎么能知道代理类被使用呢？
// 软件使用它们的时候并不可能会告知它们的存在，它们是不可见的，至少在概念上！
// 一旦发现了它们，难道就必须放弃使用auto和条款5所中的auto的各种好处吗？

// 先看看怎么解决如何发现它们的问题。
// 尽管不可见的代理类被设计用来fly beneath programmer raddat in day-to-day use，
// 库使用它们的时候常常会撰写关于它们的文档来解释为什么会这么做。
// 对你所使用的库的基础设计理念越熟悉，就越不可能在这些库中被代理的使用搞得狼狈不堪。
// 当文档不够用的时候，头文件可以弥补空缺，很少有源代码封装一个完全的代理类。
// 它们常常从一些客户调用者期望调用的函数返回，所有函数签名常常可以表征它们的存在。
// 这里是std::vector<bool>::operator[]的例子：
// namespace std {
// from C++ Standards
// template <class Allocator>
// class vector<bool, Allocator> {
//  public:
//   … class reference { … };
//   reference operator[](size_type n);
//   …
// };
// }

// 假设知道对std::vector<T>的operator[]常常返回一个T&，
// 在这个例子中的这种非常规的operator[]的返回类型一般就表征了代理类的使用。
// 在你正在使用的这些接口之上加以关注常常可以发现代理类的存在，在实践上，
// 很多的开发者只会在尝试修复奇怪的编译问题或者是调试错误的单元测试结果中发现代理类的使用。
// 不管是如何发现它们，一旦auto被决定作为推导代理类的类型而不是它被代理的类型，
// 它就不需要涉及到关于auto，auto自己本身没有问题。
// 问题在于auto推导的类型不是所想让它推导出来的类型。
// 解决方案就是强制一个不同的类型推导，我把这种方法叫做显式的类型初始化原则。
// 显式的类型初始化原则涉及到使用auto声明一个变量，但是转换初始化表达式到auto想要的类型。
// 下面就是一个强制highPriority类型是bool的例子：
// auto highPriority = static_cast<bool>(features(w)[5]);

// 这里，features(w)[5]还是返回一个std::vector<bool>::reference的对象，
// 就和它经常的表现一样，但是强制类型转换改变了表达式的类型成为bool，
// 然后auto才推导其作为highPriority的类型，在运行的时候，
// 从std::vector<bool>::operator[]返回的std::vector<bool>::reference对象，
// 支持执行转换到bool的行为，作为转换的一部分，
// 从features返回的仍然存活的指向std::vector<bool>的指针被间接引用。
// 这样就在运行的开始避免了未定义行为，索引5然后放置在bits指针的偏移上，
// 然后暴露的bool就作为highPriority的初始化数值。
// 针对于Matrix的例子，显示的类型初始化原则可能会看起来是这样的：
// auto sum = static_cast<Matrix>(m1 + m2 + m3 + m4);

// 关于这个原则下面的程序并不禁止初始化但是要排除代理类类型。
// 强调你要谨慎地创建一个类型的变量，它和从初始化表达式生成的类型是不同的也是有帮助意义的。
// 举一个例子，假设你有一个函数去计算一些方差：
double calcEpsilon() {}  // 返回方差
// calcEpsilon明确的返回一个double，但是假设知道程序，float的精度就够了的时候，
// 而且要关注double和float的长度的区别，可以声明一个float变量去存储calcEpsilon的结果：
float ep1 = calcEpsilon();  // 隐式转换double到float
// 但是这个很难表明故意减小函数返回值的精度，一个使用显式的类型初始化原则式这样做的：
auto ep2 = static_cast<float>(calcEpsilon());
}  // namespace n2

// 要记住的东西：

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 0]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}