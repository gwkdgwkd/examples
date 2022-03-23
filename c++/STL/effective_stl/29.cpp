#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

// 对于逐个字符的输入请考虑使用istreambuf_iterator

int main() {
  // 想把一个文本文件的内容拷贝到一个sting对象中：
  std::ifstream inputFile("data.txt");
  std::string fileData((std::istream_iterator<char>(inputFile)),
                       std::istream_iterator<char>());
  // 这段代码并没有把文件中的空白字符拷贝到string对象中。
  // 因为istream_iterator使用operator>>函数完成实际的读操作，而默认情况下operator>>函数会跳过空白字符。

  // 修改默认行为，清除输入流的skipws标志即可：
  inputFile.unsetf(std::ios::skipws);
  std::string fileData1((std::istream_iterator<char>(inputFile)),
                        std::istream_iterator<char>());
  // 现在inputFile中的所有字符都会被拷贝到fileData中。

  // 拷贝过程不够快，istream_iterator内部使用的operator>>函数实际执行了格式化的输入，有很多附加操作：
  //  一个内部的sentry对象的构造和析构；
  //  检查那些可能会影响其行为的流标志；
  //  检查所有可能发生的读取错误；
  //  如果遇到错误的话，还需要检查输入流的异常屏蔽标志以决定是否抛出相应的异常。

  // 有一种更为有效的途径，STL中最为神秘的法宝之一：istreambuf_iterator。
  // istreambuf_iterator的使用方法与istream_iterator大致相同，
  // 但是istream_iterator<char>对象使用operator>>从输入流中读取单个字符，
  // 而istreambuf_iterator<char>则直接从流的缓存区中读取下一个字符。
  std::string fileData2((std::istreambuf_iterator<char>(inputFile)),
                        std::istreambuf_iterator<char>());
  // 不用清除skipws标志，因为istreambuf_iterator不会跳过任何字符，
  // 只是简单地取回流缓冲区中的下一个字符，而不管它们是什么字符。

  // istreambuf_iterator比istream_iterator提高了近40%。当然，不同的STL实现其速度提高也会有所不同。
  // 使用的少，istreambuf_iterator还有优化空间。
  // 对于非格式化的逐个字符的输出过程，也应该考虑使用ostreambuf_iterator。

  return 0;
}