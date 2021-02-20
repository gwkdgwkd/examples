#include <uchar.h>
#include <cstring>
#include <iomanip>
#include <iostream>

using namespace std;

// 在C++98中，为了支持Unicode字符，使用wchar_t类型来表示“宽字符”，但并没有严格规定位宽，而是让wchar_t的宽度由编译器实现，
// 因此不同的编译器有着不同的实现方式，GNU C++规定wchar_t为32位，Visual C++规定为16位。由于wchar_t宽度没有一个统规定，
// 导致使用wchar_t的代码在不同平台间移植时，可能出现问题。这一状况在C++11中得到了一定的改善，从此Unicode字符的存储有了统一类型：
//  1 char16_t：用于存储UTF-16编码的Unicode字符。
//  2 char32_t：用于存储UTF-32编码的Unicode字符。
// 至于UTF-8编码的Unicode数据，C++11还是使用了8bits宽度的char类型数组来表示，而char16_t和char32_t的宽度由其名称可以看出，
// char16_t为16bits，char32_t为32bits。

// 除了使用新类型char16_t与char32_t来表示Unicode字符，此外，C++11还新增了三种前缀来定义不同编码的字符串，新增前缀如下：
//  1 u8表示为UTF-8编码；
//  2 u表示为UTF-16编码；
//  3 U表示为UTF-32编码。
// C++98中有两种定义字符串的方式，一是直接使用双引号定义多字节字符串，二是通过前缀“L”表示wchar_t字符串（宽字符串）。至此，C++中共有5种定义字符串的方式。

// 在使用不同方式定义不同编码的字符串时，我们需要注意影响字符串处理和显示的几个因素有编辑器、编译器和输出环境。
// 代码编辑器采用何种编码方式决定了字符串最初的编码，比如编辑器如果采用GBK，那么代码文件中的所有字符都是以GBK编码存储。
// 当编译器处理字符串时，可以通过前缀来判断字符串的编码类型，如果目标编码与原编码不同，则编译器会进行转换，比如C++11中的前缀u8表示目标编码为UTF-8的字符，
// 如果代码文件采用的是GBK，编译器按照UTF-8去解析字符串常量，则可能会出现错误。

// 一个字符串从定义到处理再到输出，涉及到编辑器、编译器和输出环境三个因素，正确的处理和显示需要三个因素的共同保障，每一个环节都不能出错。

// C++11在标准库中增加了一些Unicode编码转换的函数，开发人员可以使用库中的一些新增编码转换函数来完成各种Unicode编码间的转换，函数原型如下：
//多字节字符转换为UTF-16编码
// size_t mbrtoc16(char16_t* pc16, const char* pmb, size_t max, mbstate_t* ps);
//UTF-16字符转换为多字节字符
// size_t c16rtomb(char* pmb, char16_t c16, mbstate_t* ps);
//多字节字符转换为UTF-32编码
// size_t mbrtoc32(char32_t* pc32, const char* pmb, size_t max, mbstate_t* ps);
//UTF-32字符转换为多字节字符
// size_t c32rtomb(char* pmb, char32_t c32, mbstate_t* ps);
// 函数名称中mb表示multi-byte（多字节），rto表示convert to（转换为），c16表示char16_t，了解这些，可以根据函数名称直观的理解它们的作用。

// 实际上C++提供了一个类模板codecvt用于完成Unicode字符串与多字节字符串之间的转换，主要分为4种：
// codecvt<char,char,mbstate_t>     //performs no conversion
// codecvt<wchar_t,char,mbstate_t>  //converts between native wide and narrow character sets
// codecvt<char16_t,char,mbstate_t> //converts between UTF16 and UTF8 encodings, since C++11
// codecvt<char32_t,char,mbstate_t> //converts between UTF32 and UTF8 encodings,since C++11

// C++11新增了UTF-16和UTF-32编码的字符类型char16_t和char32_t，当然少不了对应的字符串类型，分别是u16string与与u32string，
// 二者的存在类似与string与wstring。四者的定义如下：
// typedef basic_string<char> string;
// typedef basic_string<wchar_t> wstring;
// typedef basic_string<char16_t> u16string;
// typedef basic_string<char32_t> u32string;

int main() {
  const char* sTest = u8"你好";
  for (int i = 0; sTest[i] != 0; ++i) {
    cout << setiosflags(ios::uppercase) << hex << (uint32_t)(uint8_t)sTest[i]
         << " ";
  }
  cout << endl;

  // 程序输出结果：C4 E3 BA C3。这个码值是GBK的码值，因为“你”的GBK码值是0xC4E3，“好”的GBK码值是0xBAC3。可见，编译器未成功地将GBK编码的“你好”转换
  // 为UTF-8的码值“你”（E4 BD A0）“好”（E5 A5 BD），原因是使用编译选项-finput-charset=utf-8指定代码文件编码为UTF-8，而实际上代码文件编码为GBK，
  // 导致编译器出现错误的认知。如果使用-finput-charset=gbk，那么编译器在编译时会将GBK编码的“你好”转换为UTF-8编码，正确输出E4 BD A0 E5 A5 BD。

  // 当然如果想避开编辑器编码对字符串的影响，可以使用Unicode码值来定义字符串常量:
  const char* sTest1 = u8"\u4F60\u597D";
  // 你好的Uunicode码值分别是：0x4F60和0x597D
  for (int i = 0; sTest1[i] != 0; ++i) {
    cout << setiosflags(ios::uppercase) << hex << (uint32_t)(uint8_t)sTest1[i]
         << " ";
  }
  cout << endl;
  cout << sTest1 << endl;

  // 使用c16rtomb()完成了将“你好啊”从UTF-16编码到多字节编码（GBK）的转换。
  const char16_t* utf16 = u"\u4F60\u597D\u554A";
  size_t utf16Len = char_traits<char16_t>::length(utf16);

  char* gbk = new char[utf16Len * 2 + 1];
  memset(gbk, 0, utf16Len * 2 + 1);
  char* pGbk = gbk;

  setlocale(LC_ALL, "zh_CN.gbk");
  mbstate_t mbs;  // 转换状态
  size_t length = 0;
  while (*utf16) {
    pGbk += length;
    length = c16rtomb(pGbk, *utf16, &mbs);
    if (length == 0 || pGbk - gbk > sizeof(gbk)) {
      cout << "failed" << endl;
      break;  // 转换失败
    }
    ++utf16;
  }
  for (int i = 0; gbk[i] != 0; ++i) {
    cout << setiosflags(ios::uppercase) << hex << (uint32_t)(uint8_t)gbk[i]
         << " ";
  }
  // 上面的转换，我们用到了locale机制。locale表示的是一个地域的特征，包括字符编码、数字时间表示形式、货币符号等。
  // locale串使用“zh_CN.gbk”表示目的多字节字符串使用GBK编码。

  // failed, 不是GBK编码的文件

  u16string u16str = u"\u4F60\u597D\u554A";  // 你好啊
  cout << u16str.length() << endl;           // 字符数
  for (int i = 0; i < u16str.length(); ++i) {
    cout << setiosflags(ios::uppercase) << hex << (uint16_t)u16str[i] << " ";
  }
  cout << endl;

  // 3
  // 4F60 597D 554A

  return 0;
}

// g++ -std=c++11 unicode.cpp -finput-charset=utf-8