#include <iostream>

using namespace std;

// 让自己熟悉包括TR1在内的标准程序库

// C++98列入的C++标准程序库有哪些主要成分：
// 1.STL，覆盖容器、迭代器、算法、函数对象、各种容器适配器和函数对象适配器。
// 2.iostreams，覆盖用户自定义缓冲功能、国际化I/O，
//   以及预先定义好的对象cin，cout，cerr和clog。
// 3.国际化支持，包括多区域能力。
//   像wchar_t和wstring等类型都对促进Unicode有所帮助。
// 4.数值处理，包括复数模板和纯数值数组。
// 5.异常阶层体系，
//   包括base class exception、derived class logic_error和runtime_error，
//   以及更深继承的各个class。
// 6.C89标准程序库，1989C标准程序库内的每个东西也都被覆盖于C++内。

// TR1详细叙述了14个新组件，统统都放在std命名空间内，
// 更正确地说是在其嵌套命名空间tr1内：
// 1.智能指针shared_ptr和weak_ptr。
// 2.function，此物表示任何可调用物，也就是任何函数或函数对象。
// 3.bind，它能够做STL绑定器bind1st和bind2nd所做的每一件事，而又更多。
// 其他TR1组件分为两组，第一组提供彼此互不相干的独立机能：
// 4.Hash table，用来实现set，multiset，map和multi-map。
// 5.正则表达式，包括以正则表达式为基础的字符串查找和替换，
//   或是从某个匹配字符串到另一个匹配字符串的逐一迭代等等。
// 6.Tuples(变量组)，pair只能持有两个对象，tuple可持有任意个数的对象。
// 7.array，本质是个STL化数组，即一个支持成员函数如begin和end的数组。
// 8.mem_fn，是语句构造上与成员函数指针一致的东西。
// 9.referenc_wrapper,一个让引用的行为更像对象的设施。
// 10.随机数生成工具，大大超越了rand。
// 11.数学特殊函数，包括Laguerre多项式、Bessel函数、完全椭圆积分，
//    以及更多数学函数。
// 12.C99兼容扩充，这是一大堆函数和模板，用来将许多新的C99程序特性带进C++。
// 第二组由更精巧的template编程技术（包括模板元编程）构成：
// 13.Type traits，一组traits class，用以提供类型的编译期信息。
// 14.result_of，这是个template，用来推导函数调用的返回类型。

// 请记住：
// C++标准程序库的主要机能由STL、iostream、locale组成。
// 并包括C99标准程序库。
// TR1添加了智能指针、一般化函数指针、hash-based容器、
// 正则表达式以及另外10个组件的支持。
// TR1自身只是一份规范。
// 为获得TR1提供的好处，你需要一份实物。
// 一个好的实物来源是Boost。

int main() { return 0; }