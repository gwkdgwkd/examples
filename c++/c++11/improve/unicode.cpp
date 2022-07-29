#include <uchar.h>
#include <cstring>
#include <iomanip>
#include <iostream>

// 在使用不同方式定义不同编码的字符串时，
// 需要注意影响字符串处理和显示的几个因素有编辑器、编译器和输出环境。
// 代码编辑器采用何种编码方式决定了字符串最初的编码，比如编辑器如果采用GBK，
// 那么代码文件中的所有字符都是以GBK编码存储。
// 当编译器处理字符串时，可以通过前缀来判断字符串的编码类型，如果目标编码与原编码不同，
// 则编译器会进行转换，比如C++11中的前缀u8表示目标编码为UTF-8的字符，
// 如果代码文件采用的是GBK，编译器按照UTF-8去解析字符串常量，则可能会出现错误。
// 一个字符串从定义到处理再到输出，涉及到编辑器、编译器和输出环境三个因素，
// 正确的处理和显示需要三个因素的共同保障，每一个环节都不能出错。

namespace n1 {
// 在C++98中，为了支持Unicode字符，使用wchar_t类型来表示“宽字符”，
// 但并没有严格规定位宽，而是让wchar_t的宽度由编译器实现，
// GNU C++规定wchar_t为32位，Visual C++规定为16位。
// 由于wchar_t宽度没有一个统规定，
// 导致使用wchar_t的代码在不同平台间移植时，可能出现问题。

// 这一状况在C++11中得到了一定的改善，从此Unicode字符的存储有了统一类型：
// 1.char16_t：用于存储UTF-16编码的Unicode字符。
// 2.char32_t：用于存储UTF-32编码的Unicode字符。
// 3.至于UTF-8编码的Unicode数据，C++11还是使用了8bits宽度的char类型数组来表示。

void testN1() {
  // C++98中有两种定义字符串的方式：
  auto s1 = "hello";  // 直接使用双引号定义多字节字符串
  auto s2 = L"hello";  // 通过前缀“L”表示wchar_t字符串（宽字符串）

  // 此外，C++11还新增了三种前缀来定义不同编码的字符串：
  auto s3 = u8"hello";  // UTF-8编码
  auto s4 = u"hello";   // UTF-16编码
  auto s5 = U"hello";   // UTF-32编码
}
}  // namespace n1

namespace n2 {
// C++11在标准库中增加了一些Unicode编码转换的函数，函数原型如下：
// 多字节字符转换为UTF-16编码
// size_t mbrtoc16(char16_t* pc16, const char* pmb, size_t max, mbstate_t* ps);
// UTF-16字符转换为多字节字符
// size_t c16rtomb(char* pmb, char16_t c16, mbstate_t* ps);
// 多字节字符转换为UTF-32编码
// size_t mbrtoc32(char32_t* pc32, const char* pmb, size_t max, mbstate_t* ps);
// UTF-32字符转换为多字节字符
// size_t c32rtomb(char* pmb, char32_t c32, mbstate_t* ps);
// 函数名称中mb表示multi-byte（多字节），rto表示convert to（转换为），c16表示char16_t。

// 实际上C++提供了一个类模板codecvt用于完成Unicode字符串与多字节字符串之间的转换，
// 主要分为4种：
// 1.performs no conversion
// codecvt<char,char,mbstate_t>
// 2.converts between native wide and narrow character sets
// codecvt<wchar_t,char,mbstate_t>
// 3.converts between UTF16 and UTF8 encodings, since C++11
// codecvt<char16_t,char,mbstate_t>
// 4.converts between UTF32 and UTF8 encodings,since C++11
// codecvt<char32_t,char,mbstate_t>

// C++11新增了UTF-16和UTF-32编码的字符类型char16_t和char32_t，
// 当然少不了对应的字符串类型，分别是u16string与与u32string，
// 二者的存在类似与string与wstring。四者的定义如下：
// typedef basic_string<char> string;
// typedef basic_string<wchar_t> wstring;
// typedef basic_string<char16_t> u16string;
// typedef basic_string<char32_t> u32string;

void func1() {
  std::string s1 = "nihao";
  std::wstring s2 = L"nihao";
  std::string s3 = u8"nihao";
  std::u16string s4 = u"nihao";
  std::u32string s5 = U"nihao";
}

void func2() {
  const char* s = u8"你好";
  for (int i = 0; s[i] != 0; ++i) {
    std::cout << std::setiosflags(std::ios::uppercase) << std::hex
              << (uint32_t)(uint8_t)s[i] << " ";
  }
  std::cout << std::endl;

  // g++ -std=c++11 unicode.cpp -finput-charset=utf-8
  // C4 E3 BA C3

  // g++ -std=c++11 unicode.cpp -finput-charset=gbk
  // cc1plus: error: failure to convert gbk to UTF-8

  // g++ -std=c++11 unicode.cpp
  // E4 BD A0 E5 A5 BD

  // 输出结果是GBK的码值，因为“你”的GBK码值是0xC4E3，“好”的GBK码值是0xBAC3。
  // 可见，编译器未成功地将GBK编码的“你好”转换为UTF-8的码值“你”（E4 BD A0）“好”（E5 A5 BD），
  // 原因是使用编译选项-finput-charset=utf-8指定代码文件编码为UTF-8，
  // 而实际上代码文件编码为GBK，导致编译器出现错误的认知。
  // 如果使用-finput-charset=gbk，那么编译器在编译时会将GBK编码的“你好”转换为UTF-8编码，
  // 正确输出E4 BD A0 E5 A5 BD
}

void func3() {
  // 如果想避开编辑器编码对字符串的影响，可以使用Unicode码值来定义字符串常量:
  const char* s = u8"\u4F60\u597D";  // 你[0x4F60]好[0x597D]
  for (int i = 0; s[i] != 0; ++i) {
    std::cout << std::setiosflags(std::ios::uppercase) << std::hex
              << (uint32_t)(uint8_t)s[i] << " ";
  }
  std::cout << std::endl;
  std::cout << s << std::endl;

  // g++ -std=c++11 unicode.cpp -finput-charset=utf-8
  // E4 BD A0 E5 A5 BD
  // 你好

  // g++ -std=c++11 unicode.cpp
  // E4 BD A0 E5 A5 BD
  // 你好
}

void func4() {
  // 使用c16rtomb()完成了将“你好啊”从UTF-16编码到多字节编码（GBK）的转换。
  const char16_t* utf16 = u"\u4F60\u597D\u554A";
  size_t utf16Len = std::char_traits<char16_t>::length(utf16);

  char* gbk = new char[utf16Len * 2 + 1];
  memset(gbk, 0, utf16Len * 2 + 1);
  char* pGbk = gbk;

  // locale表示的是一个地域的特征，包括字符编码、数字时间表示形式、货币符号等。
  // locale串使用“zh_CN.gbk”表示目的多字节字符串使用GBK编码。
  setlocale(LC_ALL, "zh_CN.gbk");
  mbstate_t mbs;  // 转换状态
  size_t length = 0;
  while (*utf16) {
    pGbk += length;
    length = c16rtomb(pGbk, *utf16, &mbs);
    if (length == 0 || pGbk - gbk > sizeof(gbk)) {
      std::cout << "failed" << std::endl;
      break;  // 转换失败
    }
    ++utf16;
  }
  for (int i = 0; gbk[i] != 0; ++i) {
    std::cout << std::setiosflags(std::ios::uppercase) << std::hex
              << (uint32_t)(uint8_t)gbk[i] << " ";
  }

  // failed不是GBK编码的文件
}

void func5() {
  std::u16string u16str = u"\u4F60\u597D\u554A";  // 你好啊
  std::cout << u16str.length() << std::endl;      // 3
  for (int i = 0; i < u16str.length(); ++i) {
    std::cout << std::setiosflags(std::ios::uppercase) << std::hex
              << (uint16_t)u16str[i] << " ";
  }
  std::cout << std::endl;  // 4F60 597D 554A
}

void testN2() {
  func1();
  func2();
  func3();
  func4();
  func5();
}
}  // namespace n2

namespace n3 {

void testN3() {}
}  // namespace n3

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::testN1();
      break;
    case 1:
      n2::testN2();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}

// g++ -std=c++11 unicode.cpp -finput-charset=utf-8