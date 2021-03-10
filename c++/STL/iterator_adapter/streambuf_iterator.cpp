#include <iostream>
#include <iterator>

using namespace std;

// C++ STL标准库中，流迭代器又细分为输入流迭代器和输出流迭代器，流缓冲区迭代器也是如此，其又被细分为输入流缓冲区迭代器和输出流缓冲区迭代器：
//  输入流缓冲区迭代器（istreambuf_iterator）：从输入流缓冲区中读取字符元素；
//  输出流缓冲区迭代器（ostreambuf_iterator）：将连续的字符元素写入到输出缓冲区中。
// 流缓冲区迭代器和流迭代器最大的区别在于，前者仅仅会将元素以字符的形式（包括 char、wchar_t、char16_t 及 char32_t 等）读或者写到流缓冲区中，
// 由于不会涉及数据类型的转换，读写数据的速度比后者要快。
// istreambuf_iterator和ostreambuf_iterator类型迭代器的模板类也定义在<iterator>头文件，并位于std命名空间中。

// istreambuf_iterator输入流缓冲区迭代器的功能是从指定的流缓冲区中读取字符元素。
// 该类型迭代器本质是一个输入迭代器，即假设p是一个输入流迭代器，则其只能进行++p、p++、*p操作，同时迭代器之间也只能使用==和!=运算符。

// 和istreambuf_iterator输入流缓冲区迭代器恰恰相反，ostreambuf_iterator输出流缓冲区迭代器用于将字符元素写入到指定的流缓冲区中。
// 该类型迭代器本质上是一个输出迭代器，这意味着假设p为一个输出迭代器，则它仅能执行++p、p++、*p=t以及*p++=t操作。
// 和ostream_iterator输出流迭代器一样，istreambuf_iterator迭代器底层也是通过重载赋值（=）运算符实现的。
// 换句话说，即通过赋值运算符，每个赋值给输出流缓冲区迭代器的字符元素，都会被写入到指定的流缓冲区中。

int main() {
  // 创建输入流缓冲区迭代器的常用方式，有以下2种：
  // 1 通过调用istreambuf_iterator模板类中的默认构造函数，可以创建一个表示结尾的输入流缓冲区迭代器。要知道，当我们从流缓冲区中不断读取数据时，总有
  // 读取完成的那一刻，这一刻就可以用此方式构建的流缓冲区迭代器表示。
  std::istreambuf_iterator<char> end_in;
  // <>尖括号中用于指定从流缓冲区中读取的字符类型。
  // 2 可以指定要读取的流缓冲区
  std::istreambuf_iterator<char> in1{std::cin};
  // 除此之外，还可以传入流缓冲区的地址
  std::istreambuf_iterator<char> in2{std::cin.rdbuf()};
  // dbuf()函数的功能是获取指定流缓冲区的地址。
  // 无论是传入流缓冲区，还是传入其地址，它们最终构造的输入流缓冲区迭代器是一样的。

  istreambuf_iterator<char> eos;
  istreambuf_iterator<char> iit(cin);
  string mystring;
  cout << "向缓冲区输入元素：\n";
  // 不断从缓冲区读取数据，直到读取到EOF流结束符
  // 只有读取到EOF流结束符时，程序中的iit才会和eos相等。
  // 在Windows平台上，使用Ctrl+Z组合键输入^Z表示EOF流结束符(ubuntu ctrl+d),此结束符需要单独输入，或者输入换行符之后再输入才有效。
  while (iit != eos) {
    mystring += *iit++;
  }
  cout << "string：" << mystring;

  // 创建输出流缓冲区迭代器的常用方式有以下2种：
  // 1 通过传递一个流缓冲区对象，即可创建一个输出流缓冲区迭代器
  std::ostreambuf_iterator<char> out_it1(std::cout);
  // 尖括号<>中用于指定要写入字符的类型，可以是char、wchar_t、char16_t以及char32_t等。
  // 2 还可以借助rdbuf()，传递一个流缓冲区的地址，也可以成功创建输出流缓冲区迭代器：
  std::ostreambuf_iterator<char> out_it2(std::cout.rdbuf());

  std::ostreambuf_iterator<char> out_it3(std::cout);
  *out_it3 = 'S';
  *out_it3 = 'T';
  *out_it3 = 'L';
  std::string mystring1("\nhttp://c.biancheng.net/stl/\n");
  std::copy(mystring1.begin(), mystring1.end(), out_it3);

  return 0;
}