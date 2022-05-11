#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

// cout和cerr、clog的一个区别是，cout允许被重定向，而cerr和clog都不支持。
// 值得一提的是，cin也允许被重定向。
// 在默认情况下，cin只能接收从键盘输入的数据，cout也只能将数据输出到屏幕上。
// 但通过重定向，cin可以将指定文件作为输入源，即接收文件中早已准备好的数据，
// 同样cout可以将原本要输出到屏幕上的数据转而写到指定文件中。C++中，实现重定向的常用方式有3种。

// freopen()函数实现重定向：
// freopen()定义在<stdio.h>头文件中，是C语言标准库中的函数，
// 专门用于重定向输入流（包括scanf()、gets()等）和输出流（包括printf()、puts()等）。
// 值得一提的是，该函数也可以对C++中的cin和cout进行重定向。
void func1() {
  std::string name, url;
  freopen("in.txt", "r", stdin);
  std::cin >> name >> url;
  freopen("out.txt", "w", stdout);
  std::cout << name << "\n" << url;
}

// rdbuf()函数实现重定向:
// rdbuf()函数定义在<ios>头文件中，专门用于实现C++输入输出流的重定向。
// 值得一提的是，ios作为istream和ostream类的基类，rdbuf()函数也被继承，
// 因此cin和cout可以直接调用该函数实现重定向。
// rdbuf()函数的语法格式有2种，分别为：
//  streambuf * rdbuf() const;
//  streambuf * rdbuf(streambuf * sb);
// streambuf是C++标准库中用于表示缓冲区的类，该类的指针对象用于代指某个具体的流缓冲区。
// 其中，第一种语法格式仅是返回一个指向当前流缓冲区的指针；
// 第二种语法格式用于将sb指向的缓冲区设置为当前流的新缓冲区，并返回一个指向旧缓冲区的对象。
void func2() {
  std::ifstream fin("in.txt");
  std::ofstream fout("out.txt");
  std::streambuf *oldcin;
  std::streambuf *oldcout;
  char a[100];
  oldcin = std::cin.rdbuf(fin.rdbuf());
  std::cin >> a;
  printf("%s\n", a);  // nihao
  oldcout = std::cout.rdbuf(fout.rdbuf());
  std::cout << a << std::endl;
  std::cin.rdbuf(oldcin);
  std::cout.rdbuf(oldcout);
  std::cout << a << std::endl;  // nihao
  fin.close();
  fout.close();
}

// 通过控制台实现重定向:
// 以上2种方法，都是从代码层面实现输入输出流的重定向。
// 除此之外，我们还可以通过控制台实现输入输出的重定向：
// ./a.out 2 < in.txt >out1.txt
void func3() {
  std::string name, url;
  std::cin >> name >> url;
  std::cout << name << '\n' << url;
}

int main(int argc, char *argv[]) {
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