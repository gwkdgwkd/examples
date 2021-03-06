#include <iostream>

using namespace std;

// 了解异常处理的系统开销

// 为了在运行时处理异常，程序要记录大量的信息。无论执行到什么地方，程序都必须能够识别出如果在此处抛出异常的话，将要被释放哪一个对象；
// 程序必须知道每一个入口点，以便从try块中退出；对于每一个try块，他们都必须跟踪与其相关的catch子句以及这些catch子句能够捕获的异常类型。这种信息的记录不是没有代价的。
// 虽然确保程序满足异常规格不需要运行时的比较（runtime comparisons），而且当异常被抛出时也不用额外的开销来释放相关的对象和匹配正确的catch字句。
// 但是异常处理确是有代价的，即使你没有使用try，throw或catch关键字，你同样得付出一些代价。

// 先从你不使用任何异常处理特性也要付出的代价谈起。你需要空间建立数据结构来跟踪对象是否被完全构造（constructed），你也需要CPU时间保持这些数据结构不断更新。
// 这些开销一般不是很大，但是采用不支持异常的方法编译的程序一般比支持异常的程序运行速度更快所占空间也更小。

// 在理论上，你不能对这些代价进行选择：异常是C++的一部分，C++编译器必须支持异常。也就是说，当你不用异常处理时你不能让编译器生产商消除这方面的开销，
// 因为程序一般由多个独立生成的目标文件（object files）组成，只有一个目标文件不进行异常处理并不能代表其他目标文件不进行异常处理。
// 而且即使组成可执行文件的目标文件都不进行异常处理，那么还有它们所连接的程序库呢？如果程序的任何一部分使用了异常，其它部分必须也支持异常。
// 否则在运行时程序就不可能提供正确的异常处理。

// 使用异常处理的第二个开销来自于try块，无论何时使用它，也就是当你想能够捕获异常时，那你都得为此付出代价。
// 不同的编译器实现try块的方法不同，所以编译器与编译器间的开销也不一样。粗略地估计，如果你使用try块，代码的尺寸将增加5％－10％并且运行速度也同比例减慢。
// 这还是假设程序没有抛出异常，我这里讨论的只是在程序里使用try块的开销。为了减少开销，你应该避免使用无用的try块。

// 编译器为异常规格生成的代码与它们为try块生成的代码一样多，所以一个异常规格一般花掉与try一样多的系统开销。

// 与一个正常的函数返回相比，通过抛出异常从函数里返回可能会慢三个数量级。这个开销很大。但是仅仅当你抛出异常时才会有这个开销，一般不会发生。
// 但是如果你用异常表示一个比较普遍的状况，例如完成对数据结构的遍历或结束一个循环，那你必须重新予以考虑。

// 不论异常处理的开销有多大我们都得坚持只有必须付出时才付出的原则。为了使你的异常开销最小化，只要可能就尽量采用不支持异常的方法编译程序，
// 把使用try块和异常规格限制在你确实需要它们的地方，并且只有在确为异常的情况下（exceptional）才抛出异常。
// 如果你在性能上仍旧有问题，总体评估一下你的软件以决定异常支持是否是一个起作用的因素。
// 如果是，那就考虑选择其它的编译器，能在 C++异常处理方面具有更高实现效率的编译器。

int main() { return 0; }