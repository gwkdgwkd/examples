#include <iostream>
#include <memory>

using namespace std;

// 设计class犹如设计type

// 设计优先的class是一项艰巨的工作，因为设计好的type是一项艰巨的工作。
// 好的type有自然的语法，直观的语义，以及一或多个高效实现品。

// 如何设计高效的class呢？几乎每个class都要求面对以下提问：
// 1.新type的对象应该如何被创建和销毁？
//   这会影响到构造函数和析构函数以及内存分配函数和释放函数的设计：
//   operator new，operator new[]，
//   operator delete和operator delete[]。
// 2.对象的初始化和对象的赋值该有什么样的差别？
//   这会决定构造函数和赋值操作符的行为，以及其间的差异。
//   别混淆了初始化和赋值，因为它们对应于不同的函数调用。
// 3.新type的对象如果被以值传递，意味着什么？
//   记住，copy构造函数用来定义一个type的以值传递该如何实现。
// 4.什么是新type的合法值？
//   对class的成员变量而言，通常只有某些数值集是有效的，
//   这些数值决定了必须维护的约束条件，也就决定了成员函数，
//   特别是构造函数、赋值操作符和所谓setter函数必须进行的错误检查工作。
//   也影响函数抛出的异常、以及（极少被使用的）函数异常明细列表。
// 5.新的type需要配合某个继承体系吗？
//   如果继承自某些既有的class，你就受到那些class的设计约束，
//   特别是virtual或non-virtual的影响。
//   如果你允许其他class继承你的class，
//   那会影响声明的函数，尤其是析构函数，是否为virtual。
// 6.你的新type需要什么样的转换？
//   有很多类型，彼此之间该有转换行为么？
//   如果希望允许类型T1能被隐式转换为类型T2，
//   就必须在T1内写一个类型转换函数，
//   或在T2内定义non-explicit-one-argument(可被单一实参调用)的构造函数。
//   如果允许explicit构造函数存在，就得写出专门负责执行转换的函数，
//   且不得为类型转换操作符或non-explicit-one-argument构造函数。
// 7.什么样的操作符和函数对此新type而言是合理的？
//   这个问题的答案决定你将为你的class声明哪些函数。
//   其中某些该是member函数，某些则否。
// 8.什么样的标准函数应该驳回？
//   那些正是必须声明为private的函数。
// 9.谁该取用新type的成员？
//   确定哪个成员为public，哪个为protected，哪个为private。
//   也帮助决定哪个class或function应该是friend，
//   以及将它们嵌套于另一个之内是否合理。
// 10.什么是新type的未声明接口？
//   对效率、异常安全性以及资源运用（多任务锁定和动态内存）提供何种保证？
//   在这些方面提供的保证将为代码加上相应的约束条件。
// 11.新type有多么一般化？
//   或许其实并非定义一个新type，而是定义一整个type家族。
//   如果这样就不该定义一个新class，而是应该定义一个新的class template。
// 12.你真的需要一个新type吗？
//   如果只是定义新的继承类以便为既有的class添加功能，
//   那么说不定单纯定义一或多个non-member函数或template，更能达到目标。

// 这些问题不容易回答，所以定义出高效的class是一种挑战。
// 如果能够设计出至少像C++内置类型一样好的用户自定义class，一切汗水便都值得。

// 请记住：
// 1.class的设计就是type的设计。
// 2.在定义一个新type之前，需要确定已经考虑过本条款覆盖的所有主题。

int main() { return 0; }