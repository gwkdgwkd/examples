#include <iostream>

// 条款28：理解引用折叠

struct Widget {};
// 返回右值的函数：
Widget widgetFactory() { return Widget(); }

namespace n1 {
// 实参在传递给函数模板时，推导出来的模板形参会将实参是左值还是右值的信息编码到结果类型中，
// 但这个编码操作只有在实参被用以初始化的形参为万能引用时才会发生。

// 模板形参T的推导结果类型中，会把传给param的实参是左值还是右值的信息给编码进去：
template <typename T>
void f(T&& param) {
  // 编码机制是直截了当的：
  // 1.如果传递的实参是个左值，T的推导结果就是个左值引用类型；
  // 2.如果传递的实参是个右值，T的推导结果就是个非引用类型。
}

void func1() {
  Widget w;  // 变量，左值
  f(w);      // 调用f并传入左值，T的推导结果类型为Widget&
  f(widgetFactory());  // 调用f并传入右值，T的推导结果类型为Widget

  // 两个f的调用，传递的实参类型都为Widget，不同之处仅在于，一个是左值而另一个是右值，
  // 而这个不同之处却导致了针对模板形参T得出了不同的类型推导结果。
  // 这就是决定了万能引用时编程左值引用还是右值引用的机制，也是forward得以运作的机制。
}

void func2() {
  // 在C++中，引用的引用是非法的，但如果仔细琢磨一下，
  // 当左值被传递给接受万能引用的函数模板时会发生下面的状况：
  Widget w;
  f(w);
  // 如果把T的推导结果类型，即Widget&，代码实例化模板，不就得到下面的结果了吗：
  // void f(Widget&& &param);
  // 引用的引用，然而编译器却一声未吭，条款24说由于万能引用param是用左值初始化的，
  // 其推导结果类型理应是个左值引用，但编译器是如何取用了T的推导结果类型的，
  // 并带入模板，从而使它拥有了下面这个终极版函数签名呢？
  // void f(Widget& param);

  // 答案就是引用折叠，是的，程序员是被禁止声明引用的引用，
  // 但编译器却可以在特殊的语境中产生引用的引用，模板实例化就是这样的语境之一。
  // 当编译器生成引用的引用时，引用折叠机制便支配了接下来发生的事情。

  // 有左值和右值两种引用，所以就有四种可能的引用组合：
  // 1.左值-左值；
  // 2.左值-右值；
  // 3.右值-左值；
  // 4.右值-右值。
  // 如果引用的引用出现在允许的语境，该双重引用会折叠成单个引用，规则如下：
  // 1.如果任一引用为左值引用，则结果为左值引用；
  // 2.如果两个皆为右值引用，结果为右值引用。

  // 在f(w)中，将推导结果类型Widget&代入函数模板f后，
  // 产生一个指向左值引用的右值引用，然后根据引用折叠规则，结果是个左值引用。
}
}  // namespace n1

namespace n2 {
// 引用折叠是使std::forward得以运作的关键。
// 如同条款25所解释的那样，forward会针对万能引用实施，于是就会出现如下的常见用例：
template <typename T>
void f(T&& fParam) {
  someFunc(std::forward<T>(fParam));  // 将fParam转发至someFunc
}

// 由于fParam是个万能引用，就知道，传递给f的实参，即用以初始化fParam的表达式，
// 是左值还是右值的信息会被编码到类型形参T中，std::forward的任务是，
// 当且仅当编码T中的信息表明传递给实参是个右值，即T的推导结果类型是个非引用类型时，
// 对fParam这个左值实施到右值的强制类型转换。

// std::forward的一种能够完成任务但省略细节的实现：
// template <typename T>
// T&& forward(typename remove_reference<T>::type& param) {
//   return static_cast<T&&>(param);
// }
namespace test1 {
// 假设传递给函数f的实参的类型是个左值Widget，则T会被推导为Widget&类型，
// 然后对std::forward的调用就会实例化为std::forward<Widget&>，
// 而将Widget&插入std::forward的实现就会产生如下结果：
// Widget& &&forward(typename remove_reference<Widget&>::type& param) {
//   return static_cast<Widget& &&>(param);
// }

// 由于类型特征remove_reference<Widget&>::type的产生结果是Widget类型，
// 所以std::forward又变换成了下面的结果：
// Widget& && forward(Widget& param) {
//   return static_cast<Widget& &&>(param);
// }

// 引用折叠同样在返回值和强制类型转换的语境中得到实施，
// 导致实际结果调用是这样的终极版本std::forward：
// Widget& forward(Widget& param) {
//   return static_cast<Widget&>(param);
// }

// 当左值实参被传递给函数模板f时，std::forward实例化结果是：
// 接受左值引用，并返回左值引用，而std::forward内部的强制类型转换未做任何事情，
// 因为param的类型已经是Widget&了，所以再要把它强制转换到Widget&不会产生什么效果。
// 综上，被传递给std::forward的左值实参会返回一个左值引用，
// 根据定义，左值引用是左值，所以传递左值给std::forward会导致返回一个左值，符合认知。
}  // namespace test1

namespace test2 {
// 再假设传递给f的实参是右值Widget类型。
// 在此情况下，f的类型形参T的推导结果是个光秃秃的Widget，
// 因此，f内部的std::forward就成了std::forward<Widget>。

// 在std::forward的实现中，在T之处用Widget代入，就得出下面的代码：
// Widget&& forward(typename remove_reference<Widget>::type& param) {
//   return static_cast<Widget&&>(param);
// }

// 针对非引用Widget类型实施std::remove_reference会产生和起始类型相同的结果，
// 所以std::forward又变成了这样：
// Widget&& forward(Widget& param) {
//   return static_cast<Widget&&>(param);
// }
// 没有发生引用折叠，所以这也就已经是本次std::forward调用的最终实例化版本了。

// 由函数返回的右值引用是定义为右值的，所以在此情况下，
// std::forward会把f的形参fParam（左值）转换成右值。
// 最终的结果是，传递给函数f的右值实参会作为右值转发到someFunc函数，这也符合认知。
}  // namespace test2

// 在C++14中有了std::remove_reference_t，从而forward的实现得以变得更加简明扼要：
// template <typename T>
// T&& forward(remove_reference_t<T>& param) {
//   return static_cast<T&&>(param);
// }
}  // namespace n2

namespace n3 {
// 引用折叠会出现的语境有四种，第一种，最常见的一种，就是模板实例化。
// 第二种，是auto变量的类型生成，技术细节本质上和模板实例化一模一样，
// 因为auto变量的类型推导和模板的类型推导在本质上就是一模一样的，参见条款2。

template <typename T>
void f(T&& param) {}

void func() {
  Widget w;
  f(w);
  f(widgetFactory());

  // 用auto模仿上面的行为：

  auto&& w1 = w;
  // 初始化w1的是个左值，因此auto的类型推导结果为Widget&，
  // 在w1声明中以Widget&代入auto,就产生了下面的代码：
  // Widget& && w1 = w;
  // 引用折叠之后，又会变成：
  // Widget& w1 = w;
  // 这就是结果就是，w1乃是左值引用。

  auto&& w2 = widgetFactory();
  // 以右值初始化w2，auto的类型推导结果为非引用类型Widget，将Widget代入auto就得到：
  // Widget&& w2 = widgetFactory();
  // 这里并无引用的引用，所以到此结束，w2乃是右值引用。
}

// 条款24中的万能引用并非新的引用类型，其实它就是满足了下面两个条件的右值引用：
// 1.类型推导的过程会区别左值和右值，T类型的左值推导为T&，而T类型的右值则推导为T；
// 2.会发生引用折叠。

// 万能引用的概念是有用的，有了这个概念以后，
// 就避免了需要识别出存在引用折叠的语境，根据左值和右值的不同脑补推导过程，
// 然后再脑补针对推导的结果类型代入引用折叠发生的语境后应用引用折叠规则。
}  // namespace n3

namespace n4 {
// 四种会发生引用折叠的语境，第三种语境是生成和使用typedef和别名声明，见条款9。
// 如果在typedef的创建或者评估求值的过程中出现了引用的引用，会发生折叠引用。
// 例如，假设有个类模板Widget，内嵌一个右值引用类型的typedef：
template <typename T>
class Widget {
 public:
  typedef T&& RvalueRefToT;
};

void func() {
  // 以左值引用类型来实例化该Widget：
  Widget<int&> w;

  // 在Widget中以int& 代入T的位置，则得到如下的typedef：
  // typedef int& && RvalueRefToT;

  // 引用折叠又将上述语句化简得到：
  // typedef int& RvalueRefToT;

  // 这个结果显然表明，为typedef选择的名字也许有些名不符实：
  // 当以左值引用类型实例化Widget时，RvalueRefToT其实成了左值引用的typedef。
}
}  // namespace n4

namespace n5 {
// 最后一种会发生引用折叠的语境在于decltype的运用中。
// 如果在分析一个涉及decltype的类型过程中出现了引用的引用，怎会发生折叠引用，见条款3。
}

// 要记住的东西：
// 1.引用折叠会在四种语境中发生：
//   a.模板实例化；
//   b.auto类型生成；
//   c.创建和运用typedef和别名声明；
//   d.decltype。
// 2.当编译器在引用折叠的语境下生成引用的引用时，结果会变成单个引用，
//   如果原始的引用中有任一引用为左值引用，则结果为左值引用，否则，结果为右值引用；
// 3.万能引用就是在类型推导的过程会区别左值和右值，以及会发生引用折叠的语境中的右值引用。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func1();
      break;
    case 1:
      n1::func2();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}