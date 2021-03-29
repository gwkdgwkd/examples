#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace std;

// 输出单独字符:
// put()方法专用于向输出流缓冲区中添加单个字符，其语法格式如下： ostream＆put(char c);其中，参数c为要输出的字符。
// 该函数会返回一个ostream类的引用对象，可以理解返回的是cout的引用。这意味着，可以这样使用put()函数：cout.put(c1).put(c2).put(c3);
// 因为cout.put(c1)向输出流缓冲区中添加c1字符的同时，返回一个引用形式的cout对象，所以可以继续用此对象调用put(c2)，依次类推。
// put()函数的参数可以是字符或字符的ASCII代码（也可以是一个整型表达式）。
// 除了使用cout.put()函数输出一个字符外，还可以用putchar()函数输出一个字符。
// putchar()函数是C语言中使用的，在<stdio.h>头文件中定义，C++保留了这个函数，在<iostream>头文件中定义。

// 输出字符串：
// write()成员方法专用于向输出流缓冲区中添加指定的字符串。其语法格式如下：ostream＆write（const char *s，streamsize n）;
// 其中，s用于指定某个长度至少为n的字符数组或字符串；n表示要输出的前n个字符。
// 该函数会返回一个ostream类的引用对象，可以理解返回的是cout的引用。这意味着，我们可以像下面这样使用write()方法：
// cout.write(c1, 1).write(c2, 2).write(c3, 3);

// cout.tellp()和cout.seekp()方法:
// 无论是使用cout，cout.put()，还是cout.write()，数据都会先放到输出流缓冲区，待缓冲区刷新，数据才会输出到指定位置（屏幕或者文件中）。
// tellp()成员方法用于获取当前输出流缓冲区中最后一个字符所在的位置，其语法格式如下： streampos tellp();
// tellp()不需要传递任何参数，会返回一个streampos类型值。事实上，streampos是fpos类型的别名，而fpos通过自动类型转换，可以直接赋值给一个整形变
// 量（即short、int和long）。也就是说，在使用此函数时，我们可以用一个整形变量来接收该函数的返回值。
// 注意，当输出流缓冲区中没有任何数据时，该函数返回的整形值为0；当指定的输出流缓冲区不支持此操作，或者操作失败时，该函数返回的整形值为-1。
// seekp()方法用于指定下一个进入输出缓冲区的字符所在的位置。
// seekp()方法有如下2种语法格式：
//  ostream& seekp (streampos pos);  // 指定下一个字符存储的位置
//  ostream& seekp (streamoff off, ios_base::seekdir way);  // 通过偏移量间接指定下一个字符的存储位置
//  其中，各个参数的含义如下：
//   pos：用于接收一个正整数；
//   off：用于指定相对于way位置的偏移量，其本质也是接收一个整数，可以是正数（代表正偏移）或者负数（代表负偏移）；
//   way：用于指定偏移位置，即从哪里计算偏移量，它可以接收下面3个值：
//      ios::beg 从文件头开始计算偏移量
//      ios::end 从文件末尾开始计算偏移量
//      ios::cur 从当前位置开始计算偏移量
// seekp()方法会返回一个引用形式的ostream类对象，这意味着seekp()方法可以这样使用：cout.seekp(23) << "当前位置为：" << cout.tellp();

// cout格式化输出:
// 在某些实际场景中，我们经常需要按照一定的格式输出数据，比如输出浮点数时保留2位小数，再比如以十六进制的形式输出整数，等等。
// 对于学过C语言的读者应该知道，当使用printf()函数输出数据时，可以通过设定一些合理的格式控制符，来达到以指定格式输出数据的目的。例如%.2f表示输出浮点
// 数时保留2位小数，%#X表示以十六进制、带0X前缀的方式输出整数。
// C++通常使用cout输出数据，和printf()函数相比，cout实现格式化输出数据的方式更加多样化。一方面，cout作为ostream类的对象，该类中提供有一些成员方法，
// 可实现对输出数据的格式化；另一方面，为了方面用户格式化输出数据，C++ 标准库专门提供了一个 <iomanip> 头文件，该头文件中包含有大量的格式控制符（严格
// 意义上称为“流操纵算子”），使用更加方便。
// ostream类中包含一些可实现格式化输出的成员方法，这些成员方法都是从ios基类（以及ios_base 类）中继承来的，cout（以及cerr、clog）也能调用:
//  flags(fmtfl) 	    当前格式状态全部替换为fmtfl。注意，fmtfl可以表示一种格式，也可以表示多种格式。
//  precision(n) 	    设置输出浮点数的精度为n。
//  width(w) 	        指定输出宽度为w个字符。
//  fill(c) 	        在指定输出宽度的情况下，输出的宽度不足时用字符c填充（默认情况是用空格填充）。
//  setf(fmtfl, mask) 	在当前格式的基础上，追加fmtfl格式，并删除mask格式。其中，mask参数可以省略。
//  unsetf(mask) 	    在当前格式的基础上，删除mask格式。
// flags()函数的fmtfl参数、setf()函数中的fmtfl参数和mask参数以及unsetf()函数mask参数，可以选择这些值:
//  ios::boolapha 	把true和false输出为字符串
//  ios::left 	    输出数据在本域宽范围内向左对齐
//  ios::right 	    输出数据在本域宽范围内向右对齐
//  ios::internal 	数值的符号位在域宽内左对齐，数值右对齐，中间由填充字符填充
//  ios::dec 	    设置整数的基数为10
//  ios::oct 	    设置整数的基数为8
//  ios::hex 	    设置整数的基数为16
//  ios::showbase 	强制输出整数的基数（八进制数以0开头，十六进制数以0x打头）
//  ios::showpoint 	强制输出浮点数的小点和尾数0
//  ios::uppercase 	在以科学记数法格式E和以十六进制输出字母时以大写表示
//  ios::showpos 	对正数显示“+”号
//  ios::scientific 浮点数以科学记数法格式输出
//  ios::fixed 	    浮点数以定点格式（小数形式）输出
//  ios::unitbuf 	每次输出之后刷新所有的流
// 值得一提的是，当调用unsetf()或者2个参数的setf()函数时，为了提高编写代码的效率，可以给mask参数传递如下3个组合格式：
//  ios::adjustfield：等价于ios::left|ios::right|ios::internal；
//  ios::basefield：等价于ios::dec|ios::oct|ios::hex；
//  ios::floatfield：等价于ios::scientific|ios::fixed。

// <iomanip>头文件中定义的一些常用的格式控制符，它们都可用于格式化输出(带*的是默认选项):
// 　*dec 	            以十进制形式输出整数
// 　hex 	            以十六进制形式输出整数
// 　oct 	            以八进制形式输出整数
// 　fixed 	            以普通小数形式输出浮点数
// 　scientific 	    以科学计数法形式输出浮点数
// 　left 	            左对齐，即在宽度不足时将填充字符添加到右边
// 　*right 	        右对齐，即在宽度不足时将填充字符添加到左边
// 　setbase(b) 	    设置输出整数时的进制，b=8、10或16
// 　setw(w) 	        指定输出宽度为w个字符，或输入字符串时读入w个字符。注意，该函数所起的作用是一次性的，即只影响下一次cout输出。
// 　setfill(c) 	    在指定输出宽度的情况下，输出的宽度不足时用字符c填充（默认情况是用空格填充）
// 　setprecision(n) 	设置输出浮点数的精度为n。
// 　                   在使用非fixed且非scientific方式输出的情况下，n即为有效数字最多的位数，如果有效数字位数超过n，则小数部分四舍五人，或自动变为科学计
//                      数法输出并保留一共n位有效数字。在使用fixed方式和scientific方式输出的情况下，n是小数点后面应保留的位数。
// 　setiosflags(mask) 	在当前格式状态下，追加mask格式。
// 　resetiosflags(mask) 在当前格式状态下，删除mask格式。
// 　boolapha 	        把true和false输出为字符串
// 　*noboolalpha 	    把true和false输出为0、1
// 　showbase 	        输出表示数值的进制的前缀
// 　*noshowbase 	    不输出表示数值的进制.的前缀
// 　showpoint 	        总是输出小数点
// 　*noshowpoint   	只有当小数部分存在时才显示小数点
// 　showpos 	        在非负数值中显示+
// 　*noshowpos 	    在非负数值中不显示+
// 　uppercase 	        十六进制数中使用A~E。若输出前缀，则前缀输出0X，科学计数法中输出E
// 　*nouppercase 	    十六进制数中使用a~e。若输出前缀，则前缀输出0x，科学计数法中输出e。
// 　internal 	        数值的符号（正负号）在指定宽度内左对齐，数值右对齐，中间由填充字符填充。
// 注意，如果两个相互矛盾的标志同时被设置，如先设置setiosflags(ios::fixed)，然后又设置setiosflags(ios::scientific)，那么结果可能就是两个标志都不起作用。
// 因此，在设置了某标志，又要设置其他与之矛盾的标志时，就应该用resetiosflags清除原先的标志。

int main() {
  cout.put('a');
  cout.put(65 + 32);
  cout.put(97);
  cout.put(71).put(79).put(79).put(68).put('\n');
  // aaaGOOD

  string str = "ten.gnehcnaib.c//:ptth";
  for (int i = str.length() - 1; i >= 0; i--) {
    cout.put(str[i]);  // 从最后一个字符开始输出
  }
  cout.put('\n');
  // http://c.biancheng.net

  putchar('n');
  putchar('\n');
  // n

  const char* str1 = "http://c.biancheng.net/cplus/";
  cout.write(str1, 4) << endl;  // http
  cout.write("nihao", 5).write(",wohao", 6).write(",dajiahao\n", 10);
  // nihao,wohao,dajiahao

  // 借助cout.put()方法向test.txt文件中写入指定字符，由于此过程中字符会先存入输出流缓冲区，所以借助tellp()方法，可以实时监控新存入缓冲区中字符的位置。
  std::ofstream outfile;
  outfile.open("test.txt");
  const char* str2 = "http://c.biancheng.net/cplus/";
  for (int i = 0; i < strlen(str2); i++) {
    outfile.put(str2[i]);
    long pos = outfile.tellp();  // 获取当前输出流
    std::cout << pos << ' ';
  }
  cout << std::endl;
  // 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29
  // 关闭文件之前，刷新outfile输出流缓冲区，使所有字符由缓冲区流入test.txt文件
  outfile.close();

  ofstream outfile1;
  outfile1.open("test1.txt");
  for (int i = 0; i < strlen(str2); i++) {
    outfile1.put(str2[i]);
  }
  cout << "当前位置为：" << outfile1.tellp() << endl;  // 29
  // 调整新进入缓冲区字符的存储位置
  outfile1.seekp(23);
  // 等价于：
  // outfile.seekp(23, ios::beg);
  // outfile.seekp(-6, ios::cur);
  // outfile.seekp(-6, ios::end);
  cout << "新插入位置为:" << outfile1.tellp() << endl;  // 23
  const char* newstr = "python/";
  outfile1.write("python/", 7);
  outfile1.seekp(-10, ios::end);
  cout << "当前位置为：" << outfile1.tellp() << endl;  // 20
  outfile1.put('A');
  outfile1.seekp(-5, ios::cur);
  cout << "当前位置为：" << outfile1.tellp() << endl;  // 16
  outfile1.put('B');
  outfile1.seekp(28, ios::beg);
  cout << "当前位置为：" << outfile1.tellp() << endl;  // 28
  outfile1.put('C');
  // 关闭文件之前，刷新outfile输出流缓冲区，使所有字符由缓冲区流入test.txt文件
  outfile1.close();
  // http://c.biancheBg.nAt/pythoC/

  // 注意，当cout采用此方式(成员函数的方式？)进行格式化输出时，其后不能立即输出数据，而只能像示例程序中那样，再用一个cout输出数据。
  double a = 1.23;
  cout.precision(4);
  cout << "precision: " << a << endl;  // precision: 1.23
  cout.setf(ios::scientific);
  cout << "scientific：" << a << endl;  // scientific：1.2300e+00
  double f = 123;
  cout.setf(ios::scientific);
  cout << f << '\n';  // 1.2300e+02
  cout.unsetf(ios::floatfield);
  cout << f << endl;  // 123

  cout.setf(ios::boolalpha);
  cout << true << ' ' << false << endl;  // true false
  cout.unsetf(ios::boolalpha);
  cout << true << ' ' << false << endl;  // 1 0

  cout.width(5);
  cout.setf(ios::left);
  cout << '{' << 1 << '|' << 2 << '}' << endl;  // {    1|2}
  cout.unsetf(ios::left);
  cout << '{' << 1 << '|' << 2 << '}' << endl;  // {1|2}
  cout << '{';
  cout.width(5);
  cout.setf(ios::right);
  cout << 1 << '|' << 2 << '}' << endl;  // {    1|2}
  cout.unsetf(ios::right);

  cout << '{';
  cout.width(5);
  cout.setf(ios::internal);
  cout << -1 << '}' << endl;  // {-   1}
  cout.unsetf(ios::internal);

  // 除非知道当前没有设置基标志，否则ios::setf(_IFlags)不应和ios::dec、ios::oct或ios::hex的标志值一起使用。格式化的输入/输出函数和运算符假定
  // 只设置了一个基。改用ios_base。例如，setf(ios_base::oct,ios_base::basefield)清除所有基信息并将基设置成八进制。
  cout.setf(ios::showbase);
  cout.setf(ios::hex, ios_base::basefield);
  cout << 12 << endl;  // 0xc
  // cout.setf(ios::oct, ios_base::basefield);
  cout.setf(ios_base::oct, ios_base::basefield);
  cout << 12 << endl;  // 014
  cout.setf(ios::dec, ios_base::basefield);
  cout << 12 << endl;  // 12
  cout.unsetf(ios::showbase);

  cout.setf(ios::showpoint);
  cout << 12.3 << endl;  // 12.30

  cout.setf(ios::showbase);
  cout.setf(ios::hex, ios_base::basefield);
  cout.setf(ios::scientific | ios::uppercase);
  cout << 12 << ' ' << 12.4456 << endl;  // 0XC 1.2446E+01
  cout.unsetf(ios::scientific);
  cout.unsetf(ios::uppercase);

  cout.setf(ios::basefield);
  cout.setf(ios::showpos);
  cout << 12 << endl;  // +12
  cout.unsetf(ios::showpos);
  cout << 12 << endl;  // 12

  cout.setf(ios::unitbuf);
  cout.setf(ios::fixed);
  cout << 3.456 << endl;  // 3.4560
  cout.unsetf(ios::fixed);
  cout << 3.456 << endl;  // 3.456

  cout << hex << 16 << endl;                            // 0x10
  cout << oct << 16 << endl;                            // 020
  cout << dec << 16 << endl;                            // 16
  cout << resetiosflags(ios::basefield) << 16 << endl;  // 16
  cout << setbase(16) << 16 << endl;                    // 0x10
  cout << setbase(8) << 16 << endl;                     // 020
  cout << setbase(10) << 16 << endl;                    // 16

  cout << scientific << 123.56 << endl;                      // 1.2356e+02
  cout << resetiosflags(ios::scientific) << 123.56 << endl;  // 123.6
  cout << fixed << 123.56 << endl;                           // 123.5600
  cout << resetiosflags(ios::fixed) << 123.56 << endl;       // 123.6

  cout << '{' << setw(5) << internal << -5 << '}' << endl;            // {-   5}
  cout << '{' << setw(5) << internal << showpos << 5 << '}' << endl;  // {+   5}
  cout << '{' << setw(3) << internal << noshowpos << 5 << '}' << endl;  // {  5}

  cout << '{' << setw(5) << left << 5 << '}' << endl;     // {5    }
  cout << '{' << setw(5) << right << 5 << '}' << endl;    // {    5}
  cout << setw(5) << setfill('x') << right << 5 << endl;  // xxxx5

  cout << boolalpha << true << ' ' << false << endl;    // true false
  cout << noboolalpha << true << ' ' << false << endl;  // 1 0

  cout << hex << showbase << 16 << endl;                 // 0x10
  cout << hex << uppercase << showbase << 16 << endl;    // 0X10
  cout << hex << nouppercase << showbase << 16 << endl;  // 0x10
  cout << hex << noshowbase << 16 << endl;               // 10

  cout << showpoint << 3.15 << ' ' << 3.0 << endl;    // 3.150 3.000
  cout << noshowpoint << 3.15 << ' ' << 3.0 << endl;  // 3.15 3

  return 0;
}