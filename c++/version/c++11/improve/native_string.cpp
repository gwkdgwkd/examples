#include <iostream>
#include <string>

// 原生字符串字面量（raw string literal）并不是一个新鲜的概念，
// 在许多编程语言中，都可以看到对原生字符串字面量的支持。
// 原生字符串使用户书写的字符串所见即所得，
// 不再需要如'\t'、'\n'等控制字符来调整字符串中的格式，
// 这对编程语言的学习和使用都是具有积极意义的。
// 顺应这个潮流，在C++11中，终于引入了原生字符串字面量的支持。
// C++11中原生字符串的声明相当简单，程序员只需要在字符串前加入前缀，
// 即字母R，并在引号中用使用括号左右标识，就可以声明该字符串为原生字符串了。

int main() {
  // 一个普通的字符串，'\n'被当作是转义字符，表示一个换行符。
  std::string normal_str = "First line.\nSecond line.\nEnd of message.\n";
  std::cout << normal_str << std::endl;
  // First line.
  // Second line.
  // End of message.

  // 一个raw string，'\'不会被转义处理，"\n"表示两个字符：字符反斜杠和字母n。
  std::string raw_str = R"(First line.\nSecond line.\nEnd of message.\n)";
  std::cout << raw_str << std::endl;
  // First line.\nSecond line.\nEnd of message.\n

  // 只有括号中的生效：
  std::cout << R"foo(Hello, world!)foo" << std::endl;
  // Hello, world!

  // raw string可以跨越多行，其中的空白和换行符都属于字符串的一部分：
  std::cout << R"(
                   Hello,
                   world!
                   )"
            << std::endl;
  //
  //                    Hello,
  //                    world!
  //

  return 0;
}