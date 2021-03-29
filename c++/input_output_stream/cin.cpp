#include <iostream>

using namespace std;

// get()是istream类的成员函数，它有多种重载形式。最简单常用的：int get();此函数从输入流中读入一个字符，返回值就是该字符的ASCII码。如果碰到输入的末尾，则返回值为EOF。
// istream类中从输入流（包括文件）中读取数据的成员函数，在把输入数据都读取完后再进行读取，就会返回EOF(End of File)。EOF是在iostream类中定义的一个整型常量，值为-1。
// get()函数不会跳过空格、制表符、回车等特殊字符，所有的字符都能被读入。

int main() {
  // 程序中的变量c应为int类型，而不能是char类型。在输入流中碰到ASCII码等于0xFF的字符时，cin.get()返回0xFF，0xFF赋值给c，此时如果c是char类型的，
  // 那么其值就是-1（因为符号位为1代表负数），即等于EOF，于是程序就错误地认为输入已经结束。
  // 而在c为int类型的情况下，将0xFF赋值给c，c的值是255（因为符号位为0，是正数），而非-1，即除非读到输入末尾，c的值都不可能是-1。
  int c;
  while ((c = cin.get()) != EOF) cout.put(c);

  int c1;
  freopen("in.txt", "r", stdin);  // 将标准输入重定向为test.txt
  while ((c1 = cin.get()) != EOF) cout.put(c1);

  return 0;
}