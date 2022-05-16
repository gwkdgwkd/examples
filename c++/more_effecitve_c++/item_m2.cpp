#include <iostream>
#include <string>

using namespace std;

// 尽量使用C++风格的类型转换

// C风格的类型转换:
// 1.过于粗鲁，能允许你在任何类型之间进行转换。
//   不过如果要进行更精确的类型转换，这会是一个优点。
//   把一个指向const对象的指针转换成指向非const对象的指针，
//   把一个指向基类的指针转换成指向子类的指针（即完全改变对象类型）。
//   传统的C风格的类型转换不对上述两种转换进行区分。
//   这一点也不令人惊讶，因为C风格的类型转换是为C语言设计的，而不是为C++语言设计的。
// 2.C风格的类型转换在程序语句中难以识别。
//   在语法上，类型转换由圆括号和标识符组成，而这些可以用在C++中的任何地方。
//   人工阅读很可能忽略了类型转换的语句，而利用象grep的工具程序也不能从语句构成上区分出它们来。
// 3.也许让类型转换符不美观和键入麻烦是一件好事。

// C++通过引进四个新的类型转换操作符克服了C风格类型转换的缺点，
// 这四个操作符是static_cast,const_cast,dynamic_cast和reinterpret_cast。
// 这样的类型转换不论是对人工还是对程序都很容易识别。

// static_cast 在功能上基本上与C风格的类型转换一样强大，含义也一样。它也有功能上限制:
// 1.不能用static_cast象用C风格的类型转换一样把struct转换成int类型;
// 2.不能把double类型转换成指针类型；
// 3.static_cast不能从表达式中去除const属性，因为另一个新的类型转换操作符const_cast有这样的功能。

// const_cast用于类型转换掉表达式的const或volatileness属性。
// 通过使用const_cast，你向人们和编译器强调你通过类型转换想做的只是改变一些东西的const或者volatile属性。
// 这个含义被编译器所约束。
// 如果你试图使用const_cast来完成修改const或者volatile属性之外的事情，你的类型转换将被拒绝。
// 到目前为止，const_cast最普通的用途就是转换掉对象的const属性。
class Widget {
 public:
  virtual ~Widget() {}
};
class SpecialWidget : public Widget {};
void update(SpecialWidget *psw) {}

// 第二种特殊的类型转换符是dynamic_cast，它被用于安全地沿着类的继承关系向下进行类型转换。
// 这就是说，你能用dynamic_cast把指向基类的指针或引用转换成指向其派生类或其兄弟类的指针或引用，
// 而且你能知道转换是否成功。
// 失败的转换将返回空指针（当对指针进行类型转换时）或者抛出异常（当对引用进行类型转换时）。
// dynamic_casts在帮助你浏览继承层次上是有限制的。
// 它不能被用于缺乏虚函数的类型上，也不能用它来转换掉const。
// 如你想在没有继承关系的类型中进行转换，你可能想到static_cast。
// 如果是为了去除const，你总得用const_cast。
void updateViaRef(SpecialWidget &rsw) {}

// 使用reinterpret_cast这个操作符的类型转换，转换结果几乎都是执行期定义（implementation-defined）。
// 因此，使用reinterpret_casts的代码很难移植。
// reinterpret_casts的最普通的用途就是在函数指针类型之间进行转换。
typedef void (*FuncPtr)();  // 指向函数的指针，该函数没有参数，返回值类型为void
FuncPtr funcPtrArray[10];
int doSomething() {}  // 希望把一个指向下面函数的指针存入funcPtrArray数组
// 转换函数指针的代码是不可移植的（C++不保证所有的函数指针都被用一样的方法表示），
// 在一些情况下这样的转换会产生不正确的结果，
// 所以你应该避免转换函数指针类型，除非你处于着背水一战和尖刀架喉的危急时刻。
// 一把锋利的刀，一把非常锋利的刀。

// 如果你使用的编译器缺乏对新的类型转换方式的支持，
// 你可以用传统的类型转换方法代替static_cast, const_cast以及reinterpret_cast。
// 也可以用下面的宏替换来模拟新的类型转换语法：
#define static_cast(TYPE, EXPR) ((TYPE)(EXPR))
#define const_cast(TYPE, EXPR) ((TYPE)(EXPR))
#define reinterpret_cast(TYPE, EXPR) ((TYPE)(EXPR))
// 没有一个容易的方法来模拟dynamic_cast的操作，但是很多函数库提供了函数，
// 安全地在派生类与基类之间进行类型转换。
// 如果你没有这些函数而你有必须进行这样的类型转换，你也可以回到C风格的类型转换方法上，
// 但是这样的话你将不能获知类型转换是否失败。
// 当然，你也可以定义一个宏来模拟dynamic_cast的功能，就象模拟其它的类型转换一样：
#define dynamic_cast(TYPE, EXPR) (TYPE)(EXPR)
// 请记住，这个模拟并不能完全实现dynamic_cast的功能，它没有办法知道转换是否失败。

int main() {
  int firstNumber, secondNumber;
  double result1 = ((double)firstNumber) / secondNumber;
  double result2 = static_cast<double>(firstNumber) / secondNumber;

  SpecialWidget sw;               // sw是一个非const对象
  const SpecialWidget &csw = sw;  // csw是sw的一个引用它是一个const对象
  // 错误!不能传递一个const SpecialWidget*变量给一个处理SpecialWidget*类型变量的函数
  // update(&csw);
  // csw和sw两个变量值在update函数中能被更新
  update(const_cast<SpecialWidget *>(&csw));  // 正确，csw的const被显示地转换掉
  update((SpecialWidget *)&csw);  // 同上，但用了一个更难识别的C风格的类型转换
  Widget *pw = new SpecialWidget;
  // update(pw);  // 错误！pw的类型是Widget*，但是update函数处理的是SpecialWidget*类型

  // 错误！const_cast仅能被用在影响constness or volatileness的地方上。不能用在向继承子类进行类型转换
  // update(const_cast<SpecialWidget *>(pw));

  Widget *pw1;
  // 正确，传递给update函数一个指针是指向变量类型为SpecialWidget的pw的指针，如果pw确实指向一个对象。
  // 否则传递过去的将使空指针：
  update(dynamic_cast<SpecialWidget *>(pw1));
  // 正确，传递给updateViaRef函数SpecialWidget pw引用，如果pw确实指向了某个对象。
  // 否则将抛出异常：
  updateViaRef(dynamic_cast<SpecialWidget &>(*pw1));
  // double result = dynamic_cast<double>(firstNumber) / secondNumber;  // 错误！没有继承关系
  const SpecialWidget sw1;
  // update(dynamic_cast<SpecialWidget *>(&sw1)); // 错误!dynamic_cast不能转换掉const

  // funcPtrArray[0] = &doSomething;  // 错误！类型不匹配
  funcPtrArray[0] = reinterpret_cast<FuncPtr>(&doSomething);

  // 这些模拟不会象真实的操作符一样安全，但是当你的编译器可以支持新的的类型转换时，
  // 它们可以简化你把代码升级的过程。
  double result = static_cast(double, firstNumber) / secondNumber;
  update(const_cast(SpecialWidget *, &sw));
  funcPtrArray[0] = reinterpret_cast(FuncPtr, &doSomething);

  return 0;
}