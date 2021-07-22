#include <iostream>

using namespace std;

// 总是包含（#include）正确的头文件

// STL编程中一件极其令人沮丧的事情是，在一个STL平台上能够顺利通过编译的软件，在另一个STL平台上可能需要一些额外的#include才能通过编译。
// 这是因为，C++标准和C的标准不同，没有规定标准库中的头文件之间的相互包含关系。既然有了这样的灵活性，于是，不同的C++实现就选择了不同的实现方式。

// 任何时候只要引用了std名字空间中的元素，你就有责任包含（#include）正确的头文件。
// 如果省略了这些头文件，也许在特定的STL平台上你的代码可以通过编译，但毕竟漏掉了必要的头文件，所以，其他的STL平台完全有可能拒绝你的代码。

// STL有关的标准头文件中所包含的内容：
//  1 几乎所有的标准STL容器都被声明在与之同名的头文件，比如vector被声明在<vector>中，list被声明在<list>中，等等。
//    但<set>和<map>是个例外，<set>中声明了set和multiset，<map>中声明了map和multimap。
//  2 除了4个STL算法以外，其他所有的算法都被声明在<algorithm>中，这四个算法是accumulate、inner_product、adjactnt_difference和partial_sum，
//    它们被声明在头文件<numeric>中。
//  3 特殊类型的迭代器，包括istream_iterator和istreambuf_iterator，被声明在<iterator>中。
//  4 标准的函数子（比如less<T>）和函数子配接器（比如not1、bind2nd）被声明在头文件<functional>中。

// 任何时候使用了某个头文件中的一个STL组件，那么就一定要提供对应的#include指令，即使允许省略了，为了移植压力，也要将它们包含到你的代码中。

int main() { return 0; }
