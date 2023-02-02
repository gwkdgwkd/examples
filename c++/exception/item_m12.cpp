#include <math.h>
#include <iostream>

using namespace std;

// 理解抛出一个异常与传递一个参数或调用一个虚函数间的差异

// 从语法上看，在函数里声明参数与在catch子句中声明参数几乎没有什么差别：
class Widget {};  // 一个类，具体是什么类在这里并不重要

// 一些函数，其参数分别为Widget,Widget&,或Widget*：
void f1(Widget w);
void f2(Widget& w);
void f3(const Widget& w);
void f4(Widget* pw);
void f5(const Widget* pw);

// 一些catch子句，用来捕获异常，
// 异常的类型为Widget，Widget&或Widget*：
// catch (Widget w)
// catch (Widget& w)
// catch (const Widget& w)
// catch (Widget *pw)
// catch (const Widget *pw)
// 因此可能会认为：
// 用throw抛出异常到catch子句中与通过函数调用传递参数两者基本相同。
// 这里面确有一些相同点，但是他们也存在着巨大的差异。
// 传递函数参数与异常的途径可以是传值、传递引用或传递指针，这是相同的。
// 但是当你传递参数和异常时，系统所要完成的操作过程则是完全不同的。
// 产生这个差异的原因是：
// 你调用函数时，程序的控制权最终还会返回到函数的调用处，
// 但是当你抛出一个异常时，控制权永远不会回到抛出异常的地方。
istream operator>>(istream& s, Widget& w) {}
void passAndThrowWidget() {
  Widget localWidget;
  cin >> localWidget;  // 传递localWidget到operator>>
  throw localWidget;   // 抛出localWidget异常
}
// 当传递localWidget到函数operator>>里，不用进行拷贝操作，
// 而是把operator>>内的引用类型变量w指向localWidget，
// 任何对w的操作实际上都施加到localWidget上。
// 这与抛出localWidget异常有很大不同。
// 不论通过传值捕获异常还是通过引用捕获，都将进行lcalWidget的拷贝操作，
// 也就说传递到catch子句中的是localWidget的拷贝。
// 不能通过指针捕获这个异常，因为类型不匹配。
// 必须这么做，因为当localWidget离开了生存空间后，其析构函数将被调用。
// 如果把localWidget本身（而不是它的拷贝）传递给catch子句，
// 这个子句接收到的只是一个被析构了的Widget，
// 一个Widget的尸体，这是无法使用的。
// 因此C++规范要求被做为异常抛出的对象必须被复制。
// 即使被抛出的对象不会被释放，也会进行拷贝操作。
void passAndThrowWidget1() {
  // 静态变量（static）,一直存在至程序结束：
  static Widget localWidget;
  cin >> localWidget;  // 象以前那样运行
  throw localWidget;   // 仍将对localWidget进行拷贝操作
}

// 当抛出异常时仍将复制出localWidget的一个拷贝。
// 这表示即使通过引用来捕获异常，也不能在catch块中修改localWidget；
// 仅仅能修改localWidget的拷贝。
// 对异常对象进行强制复制拷贝，
// 这个限制有助于我们理解参数传递与抛出异常的第二个差异：
// 抛出异常运行速度比参数传递要慢。

// 当异常对象被拷贝时，拷贝操作是由对象的拷贝构造函数完成的。
// 该拷贝构造函数是对象的静态类型（static type）所对应类的拷贝构造函数，
// 而不是对象的动态类型（dynamic type）对应类的拷贝构造函数。
class SpecialWidget : public Widget {};
void passAndThrowWidget2() {
  SpecialWidget localSpecialWidget;
  // rw引用SpecialWidget它抛出一个类型为Widget的异常
  Widget& rw = localSpecialWidget;
  // 这里抛出的异常对象是Widget，即使rw引用的是一个SpecialWidget。
  // 因为rw的静态类型（static type）是Widget，而不是SpecialWidget。
  // 这种行为可能与你所期待的不一样，
  // 但是这与在其他情况下C++中拷贝构造函数的行为是一致的。
  throw rw;
}

// 异常是其它对象的拷贝，这个事实影响到你如何在catch块中再抛出一个异常。
// catch (Widget& w) { // 捕获Widget异常
//   ...     // 处理异常
//   throw;  // 重新抛出异常,让它继续传递
// }
// catch (Widget& w) {
//   ...       // 处理异常
//   throw w;  // 传递被捕获异常的拷贝
// }
// 这两个catch块的差别在于第一个catch块中重新抛出的是当前捕获的异常，
// 而第二个catch块中重新抛出的是当前捕获异常的一个新的拷贝。
// 如果忽略生成额外拷贝的系统开销，这两种方法还有差异么?当然有。
// 1.第一个块中重新抛出的是当前异常(current exception)，无论它是什么类型。
// 2.特别是如果这个异常开始就是做为SpecialWidget类型抛出的，
//   那么第一个块中传递出去的还是SpecialWidget异常，
//   即使w的静态类型(static type)是Widget。
//   这是因为重新抛出异常时没有进行拷贝操作。
//   第二个catch块重新抛出的是新异常，类型总是Widget，
//   因为w的静态类型(static type)是Widget。
//   一般来说，你应该用throw来重新抛出当前的异常，
//   因为这样不会改变被传递出去的异常类型，
//   而且更有效率，因为不用生成一个新拷贝。

// catch (Widget w)  // 通过传值捕获异常
// catch (Widget& w) // 通过传递引用捕获异常
// catch (const Widget& w) // 通过传递指向const的引用捕获异常
// 传递参数与传递异常的另一个差异：
// 一个被异常抛出的对象(总是一个临时对象)可以通过普通的引用捕获；
// 它不需要通过指向const对象的引用(reference-to-const)捕获。
// 在函数调用中不允许转递一个临时对象到一个非const引用类型的参数里，
// 但是在异常中却被允许。

// 当用传值的方式传递函数的参数，
// 制造了被传递对象的一个拷贝，并把这个拷贝存储到函数的参数里。
// 同样通过传值的方式传递一个异常时，也是这么做的。
// 通过传值捕，会建立两个被抛出对象的拷贝，
// 一个是所有异常都必须建立的临时对象，第二个是把临时对象拷贝进w中。
// catch (Widget w)
// 通过引用捕获，仍旧会建立一个被抛出对象的拷贝：拷贝同样是一个临时对象。
// 相反当通过引用传递函数参数时，没有进行对象拷贝。
// catch (Widget& w)
// 当抛出一个异常时，系统构造的(以后会析构掉)被抛出对象的拷贝数，
// 比以相同对象做为参数传递给函数时构造的拷贝数要多一个。

// 通过指针抛出异常与通过指针传递参数是相同的。
// 不论哪种方法都是一个指针的拷贝被传递。
// 但你不能认为抛出的指针是一个指向局部对象的指针，
// 因为当异常离开局部变量的生存空间时，该局部变量已经被释放。
// catch子句将获得一个指向已经不存在的对象的指针。
// 这种行为在设计时应该予以避免。
// 也就是说：必须是全局的或堆中的。

// 对象从函数调用处传递到函数参数里与从异常抛出点传递到catch子句所采用的方法不同，
// 这只是参数传递与异常传递的区别的一个方面；
// 第二个差异是在函数调用者或抛出异常者与被调用者或异常捕获者之间的类型匹配过程不同。
// 一般来说，catch子句匹配异常类型时不会进行这样的转换。
bool someFunction() {
  int i;
  double sqrtOfi = sqrt(i);  // C++允许进行从int到double的隐式类型转换
  return true;
}
void f(int value) {
  try {
    if (someFunction()) {
      throw value;  // 如果someFunction()返回真,抛出一个整形值
    }
  } catch (double d) {
    // 只处理double类型的异常，
    // 但在try块中抛出的int异常不会被处理double异常的catch子句捕获。
    // 该子句只能捕获类型真真正正为double的异常，不进行类型转换。
    // 因此如果要想捕获int异常，
    // 必须使用带有int或int&参数的catch子句。
  }
}

// 不过在catch子句中进行异常匹配时可以进行两种类型转换：
// 第一种是继承类与基类间的转换。
// 一个用来捕获基类的catch子句也可以处理派生类类型的异常。
// 例如在标准C++库(STL)定义的异常类层次中的诊断部分。
// 捕获runtime_errors异常的Catch子句可以捕获：
// range_error类型和overflow_error类型的异常;
// 可以接收根类exception异常的catch子句能捕获其任意派生类异常。
// 这种派生类与基类(inheritance_based)间的异常类型转换，
// 可以作用于数值、引用以及指针上：
// can catch errors of type runtime_error，range_error or overflow_error：
// catch (runtime_error)
// catch (runtime_error&)
// catch (const runtime_error&)
// can catch errors of type runtime_error* range_error*, or overflow_error*
// catch (runtime_error*)
// catch (const runtime_error*)
// 第二种是允许从一个类型化指针转变成无类型指针，
// 所以带有const void*指针的catch子句能捕获任何类型的指针类型异常：
// catch (const void*) // 捕获任何指针类型异常

// 传递参数和传递异常间最后一点差别是：
// catch子句匹配顺序总是取决于它们在程序中出现的顺序。
// 因此一个派生类异常可能被处理其基类异常的catch子句捕获，
// 即使同时存在有能直接处理该派生类异常的catch子句，与相同的try块相对应。
// try {
// } catch (logic_error& ex) {
//   // 这个catch块将捕获所有的logic_error异常, 包括它的派生类
// } catch (invalid_argument& ex) {
//   // 这个块永远不会被执行，
//   // 因为所有的invalid_argument异常都被上面的catch子句捕获。
// }
// 如果一个处理派生类异常的catch子句位于处理基类异常的catch子句后面，
// 编译器会发出警告，因为这样的代码在C++里通常是不合法的。
// 不过你最好做好预先防范：
// 不要把处理基类异常的catch子句放在处理派生类异常的catch子句的前面。
// 应该写成这样：
// try {
// } catch (invalid_argument& ex {
//   // 处理invalid_argument异常
// } catch (logic_error& ex) {
//   // 处理所有其它的logic_errors异常
// }

// 与上面这种行为相反，当你调用一个虚拟函数时，
// 被调用的函数位于与发出函数调用的对象的动态类型(dynamic type)最相近的类里。
// 你可以这样说虚拟函数采用最优适合法，而异常处理采用的是最先适合法。

// 综上所述，把一个对象传递给函数或一个对象调用虚拟函数与把一个对象做为异常抛出，
// 这之间有三个主要区别：
// 1.异常对象在传递时总被进行拷贝；
//   当通过传值方式捕获时，异常对象被拷贝了两次。
//   对象做为参数传递给函数时不一定需要被拷贝。
// 2.对象做为异常被抛出与做为参数传递给函数相比，
//   前者类型转换比后者要少(前者只有两种转换形式)。
// 3.catch子句进行异常类型匹配的顺序是它们在源代码中出现的顺序，
//   第一个类型匹配成功的catch将被用来执行。
//   当一个对象调用一个虚拟函数时，被选择的函数位于与对象类型匹配最佳的类里，
//   即使该类不是在源代码的最前头。

int main() { return 0; }