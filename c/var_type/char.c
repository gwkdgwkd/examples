#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

// 字符类型由单引号''包围，字符串由双引号""包围。
// 对于char类型来说，它实际上存储的就是字符的ASCII码。
// 无论在哪个字符集中，字符编号都是一个整数；从这个角度考虑，字符类型和整数类型本质上没有什么区别。
// 可以给字符类型赋值一个整数，或者以整数的形式输出字符类型。反过来，也可以给整数类型赋值一个字符，或者以字符的形式输出整数类型。

// 大部分C语言教材对中文字符的处理讳莫如深，甚至只字不提，导致很多初学者认为C语言只能处理英文，而不支持中文。
// 其实C语言是一门全球化的编程语言，它支持世界上任何一个国家的语言文化，包括中文、日语、韩语等。
// 正确地存储中文字符需要解决两个问题。
// 1) 足够长的数据类型
//    char只能处理ASCII编码中的英文字符，是因为char类型太短，只有一个字节，容纳不下我大中华几万个汉字，要想处理中文字符，必须得使用更长的数据类型。
//    一个字符在存储之前会转换成它在字符集中的编号，而这样的编号是一个整数，可以用整数类型来存储一个字符，比如unsigned short、unsigned int、unsigned long等。
// 2) 选择包含中文的字符集
//    C语言规定，对于汉语、日语、韩语等ASCII编码之外的单个字符，也就是专门的字符类型，要使用宽字符的编码方式。
//    常见的宽字符编码有UTF-16和UTF-32，它们都是基于Unicode字符集的，能够支持全球的语言文化。
// C语言推出了一种新的类型，叫做wchar_t。w是wide的首字母，t是type的首字符，wchar_t的意思就是宽字符类型。wchar_t的长度由编译器决定：
//  在微软编译器下，它的长度是2，等价于unsigned short；
//  在GCC、LLVM/Clang下，它的长度是4，等价于 unsigned int。
// wchar_t类型位于<wchar.h>头文件中，它使得代码在具有良好移植性的同时，也节省了不少内存，以后我们就用它来存储宽字符。
// 将不加L前缀的字符称为窄字符，将加上L前缀的字符称为宽字符。窄字符使用ASCII编码，宽字符使用UTF-16或者UTF-32编码。
// putchar、printf只能输出不加L前缀的窄字符，对加了L前缀的宽字符无能为力，我们必须使用<wchar.h>头文件中的宽字符输出函数，它们分别是putwchar和wprintf：
//  putwchar函数专门用来输出一个宽字符，它和putchar的用法类似；
//  wprintf是通用的、格式化的宽字符输出函数，它除了可以输出单个宽字符，还可以输出宽字符串。宽字符对应的格式控制符为%lc。
// 另外，在输出宽字符之前还要使用setlocale函数进行本地化设置，告诉程序如何才能正确地处理各个国家的语言文化。
// 给字符串加上L前缀就变成了宽字符串，每个字符都是宽字符，一律采用UTF-16或者UTF-32编码。输出宽字符串可以使 <wchar.h>头文件中的wprintf函数，对应的格式控制符是%ls。

// C语言是70年代的产物，那个时候只有ASCII，各个国家的字符编码都还未成熟，所以C语言不可能从底层支持GB2312、GBK、Big5、Shift-JIS等国家编码，也不可能支持Unicode字符集。
// 在C语言中，只有char类型的窄字符才使用ASCII编码，char类型的窄字符串、wchar_t类型的宽字符和宽字符串都不使用ASCII编码。
// 对于窄字符串，C语言并没有规定使用哪一种特定的编码，只要选用的编码能够适应当前的环境即可，所以，窄字符串的编码与操作系统和编译器有关。
// 在现代计算机中，窄字符串已经不再使用ASCII编码了，因为ASCII编码只能显示字母、数字等英文字符，对汉语、日语、韩语等其它地区的字符无能为力。
// 源文件用来保存我们编写的代码，它最终会被存储到本地硬盘，或者远程服务器，这个时候就要尽量压缩文件体积，以节省硬盘空间或者网络流量，而代码中大部分的字符都是ASCII编码中
// 的字符，用一个字节足以容纳，所以UTF-8编码是一个不错的选择。
// UTF-8兼容ASCII，代码中的大部分字符可以用一个字节保存；另外UTF-8基于Unicode，支持全世界的字符，我们编写的代码可以给全球的程序员使用，真正做到技术无国界。
// 总结:
//  对于char类型的窄字符，始终使用ASCII编码。
//  对于wchar_t类型的宽字符和宽字符串，使用UTF-16或者UTF-32编码，它们都是基于Unicode字符集的。
//  对于char类型的窄字符串，微软编译器使用本地编码，GCC、LLVM/Clang使用和源文件编码相同的编码。
//  另外，处理窄字符和处理宽字符使用的函数也不一样：
//   <stdio.h>头文件中的putchar、puts、printf函数只能用来处理窄字符；
//   <wchar.h>头文件中的putwchar、wprintf函数只能用来处理宽字符。
// 源文件需要保存到硬盘，或者在网络上传输，使用的编码要尽量节省存储空间，同时要方便跨国交流，所以一般使用UTF-8，这就是选择编码字符集的标准。

// 转义字符以\或者\x开头，以\开头表示后跟八进制形式的编码值，以\x开头表示后跟十六进制形式的编码值。对于转义字符来说，只能使用八进制或者十六进制。
// 对于ASCII编码，0~31（十进制）范围内的字符为控制字符，它们都是看不见的，不能在显示器上显示，甚至无法从键盘输入，只能用转义字符的形式来表示。

int main() {
  // 正确的写法
  char a = '1';
  char b = '$';
  char c = 'X';
  char d = ' ';  // 空格也是一个字符
  // 错误的写法
  char x = '中';  // char类型不能包含ASCII编码之外的字符
  char y = 'Ａ';  // Ａ是一个全角字符
  char z = "t";   // 字符类型应该由单引号包围

  putchar(a);
  putchar(d);
  putchar(b);
  putchar(d);
  putchar(c);
  putchar('\n');
  printf("%c %c %c\n", a, b, c);

  printf("a: %c, %d\n", a, a);
  printf("b: %c, %d\n", b, b);
  printf("c: %c, %d\n", c, c);
  printf("d: %c, %d\n", d, d);

  wchar_t a4 = L'A';           // 英文字符（基本拉丁字符）
  wchar_t b4 = L'9';           // 英文数字（阿拉伯数字）
  wchar_t c4 = L'中';          // 中文汉字
  wchar_t d4 = L'国';          // 中文汉字
  wchar_t e4 = L'。';          // 中文标点
  wchar_t f4 = L'ヅ';          // 日文片假名
  wchar_t g4 = L'♥';           // 特殊符号
  wchar_t h4 = L'༄';           // 藏文
  setlocale(LC_ALL, "zh_CN");  // 使用专门的putwchar输出宽字符

  putwchar(a4);
  putwchar(b4);
  putwchar(c4);
  putwchar(d4);
  putwchar(e4);
  putwchar(f4);
  putwchar(g4);
  putwchar(h4);
  putwchar(L'\n');  // 只能使用宽字符
  // 使用通用的wprintf输出宽字符
  wprintf(L"Wide chars: %lc %lc %lc %lc %lc %lc %lc %lc\n",  // 必须使用宽字符串
          a4, b4, c4, d4, e4, f4, g4, h4);

  wchar_t web_url[] = L"http://c.biancheng.net";
  wchar_t *web_name = L"C语言中文网";
  wprintf(L"web_url: %ls \nweb_name: %ls\n", web_url, web_name);

  char a5 = '\61';                                      // 字符1
  char b5 = '\141';                                     // 字符a
  char c5 = '\x31';                                     // 字符1
  char d5 = '\x61';                                     // 字符a
  char *str1 = "\x31\x32\x33\x61\x62\x63";              // 字符串"123abc"
  char *str2 = "\61\62\63\141\142\143";                 // 字符串"123abc"
  char *str3 = "The string is: \61\62\63\x61\x62\x63";  // 混用
  puts("\x68\164\164\x70://c.biancheng.\x6e\145\x74");
  // http://c.biancheng.net

  return 0;
}