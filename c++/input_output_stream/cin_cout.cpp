#include <iostream>

using namespace std;

// 在C语言中，我们通常会使用scanf和printf来对数据进行输入输出操作。在C++语言中，C语言的这一套输入输出库我们仍然能使用，但是C++又增加了一套新的、更容易使用的输入输出库。
// C++中的输入与输出可以看做是一连串的数据流，输入即可视为从文件或键盘中输入程序中的一串数据流，而输出则可以视为从程序中输出一连串的数据流到显示屏或文件中。
// 在编写C++程序时，如果需要使用输入输出时，则需要包含头文件iostream，它包含了用于输入输出的对象，例如常见的cin表示标准输入、cout表示标准输出、cerr表示标准错误。
// cout和cin都是C++的内置对象，而不是关键字。C++库定义了大量的类（Class），程序员可以使用它们来创建对象，cout和cin就分别是ostream和istream类的对象，只不过它们是由
// 标准库的开发者提前创建好的，可以直接拿来使用。这种在C++中提前创建好的对象称为内置对象。
// 使用cout进行输出时需要紧跟<<运算符，使用cin进行输入时需要紧跟>>运算符，这两个运算符可以自行分析所处理的数据类型，因此无需像使用scanf和printf那样给出格式控制字符串。
// endl表示换行，与C语言里的\n作用相同。
// cout能够连续地输出。同样cin也是支持对多个变量连续输入的。
// cout、cin的用法非常强大灵活，推荐使用cin、cout，它们比C语言中的scanf、printf更加灵活易用。

int main() { return 0; }