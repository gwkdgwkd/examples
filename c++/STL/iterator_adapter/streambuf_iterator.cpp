#include <iostream>
#include <iterator>

// C++ STL标准库中，流迭代器又细分为输入流迭代器和输出流迭代器，流缓冲区迭代器也是如此，
// 其又被细分为输入流缓冲区迭代器和输出流缓冲区迭代器：
// 1.输入流缓冲区迭代器（istreambuf_iterator）：从输入流缓冲区中读取字符元素；
// 2.输出流缓冲区迭代器（ostreambuf_iterator）：将连续的字符元素写入到输出缓冲区中。
// 流缓冲区迭代器和流迭代器最大的区别在于，
// 前者仅仅会将元素以字符的形式（包括char、wchar_t、char16_t及char32_t等）读或者写到流缓冲区中，
// 由于不会涉及数据类型的转换，读写数据的速度比后者要快。
// istreambuf_iterator和ostreambuf_iterator类型迭代器的模板类也定义在<iterator>头文件，并位于std命名空间中。

// istreambuf_iterator输入流缓冲区迭代器的功能是从指定的流缓冲区中读取字符元素。
// 该类型迭代器本质是一个输入迭代器，即假设p是一个输入流迭代器，
// 则其只能进行++p、p++、*p操作，同时迭代器之间也只能使用==和!=运算符。

// 和istreambuf_iterator输入流缓冲区迭代器恰恰相反，
// ostreambuf_iterator输出流缓冲区迭代器用于将字符元素写入到指定的流缓冲区中。
// 该类型迭代器本质上是一个输出迭代器，这意味着假设p为一个输出迭代器，则它仅能执行++p、p++、*p=t以及*p++=t操作。
// 和ostream_iterator输出流迭代器一样，istreambuf_iterator迭代器底层也是通过重载赋值（=）运算符实现的。
// 换句话说，即通过赋值运算符，每个赋值给输出流缓冲区迭代器的字符元素，都会被写入到指定的流缓冲区中。

void func1() {
  // 创建输入流缓冲区迭代器的常用方式，有以下2种：
  // 1.通过调用istreambuf_iterator模板类中的默认构造函数，可以创建一个表示结尾的输入流缓冲区迭代器。
  // 要知道，当我们从流缓冲区中不断读取数据时，总有读取完成的那一刻，这一刻就可以用此方式构建的流缓冲区迭代器表示。
  std::istreambuf_iterator<char> eos;
  // <>尖括号中用于指定从流缓冲区中读取的字符类型。

  // 2.可以指定要读取的流缓冲区
  std::istreambuf_iterator<char> in{std::cin};
  std::string str;
  while (in != eos) {
    str += *in++;
  }
  std::cout << "str1:" << str;
}
void func2() {
  std::istreambuf_iterator<char> eos;
  // 除此之外，还可以传入流缓冲区的地址，dbuf()函数的功能是获取指定流缓冲区的地址。
  // 无论是传入流缓冲区，还是传入其地址，它们最终构造的输入流缓冲区迭代器是一样的。
  std::istreambuf_iterator<char> in{std::cin.rdbuf()};
  std::string str;
  while (in != eos) {
    str += *in++;
  }
  std::cout << "str2:" << str;
}

void func3() {
  std::string str("world\n");

  // 创建输出流缓冲区迭代器的常用方式有以下2种：
  // 1.通过传递一个流缓冲区对象，即可创建一个输出流缓冲区迭代器
  std::ostreambuf_iterator<char> out_it1(std::cout);
  // 尖括号<>中用于指定要写入字符的类型，可以是char、wchar_t、char16_t以及char32_t等。
  *out_it1 = 'H';
  *out_it1 = 'E';
  *out_it1 = 'L';
  *out_it1 = 'L';
  *out_it1 = 'O';
  *out_it1 = ',';
  std::copy(str.begin(), str.end(), out_it1);  // HELLO,world

  // 2.还可以借助rdbuf()，传递一个流缓冲区的地址，也可以成功创建输出流缓冲区迭代器：
  std::ostreambuf_iterator<char> out_it2(std::cout.rdbuf());
  *out_it2 = 'h';
  *out_it2 = 'e';
  *out_it2 = 'l';
  *out_it2 = 'l';
  *out_it2 = 'o';
  *out_it2 = ',';
  std::copy(str.begin(), str.end(), out_it2);  // hello,world
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      func1();
      break;
    case 1:
      func2();
      break;
    case 2:
      func3();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}