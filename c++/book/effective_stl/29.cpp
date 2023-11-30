#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

// 对于逐个字符的输入请考虑使用istreambuf_iterator

std::ifstream inputFile("data.txt");  // 把文件的内容拷贝到sting中

namespace n1 {
void func() {
  std::string fileData((std::istream_iterator<char>(inputFile)),
                       std::istream_iterator<char>());
  // 这段代码并没有把文件中的空白字符拷贝到string对象中。
  // 因为istream_iterator使用operator>>函数完成实际的读操作，
  // 而默认情况下operator>>函数会跳过空白字符。
}
}  // namespace n1

namespace n2 {
void func() {
  // 修改默认行为，清除输入流的skipws标志即可：
  inputFile.unsetf(std::ios::skipws);
  std::string fileData1((std::istream_iterator<char>(inputFile)),
                        std::istream_iterator<char>());
  // 现在inputFile中的所有字符都会被拷贝到fileData中。

  // 但效率地，istream_iterator使用operator>>执行格式化的输入，有很多附加操作：
  // 1.一个内部的sentry对象的构造和析构；
  // 2.检查那些可能会影响其行为的流标志；
  // 3.检查所有可能发生的读取错误；
  // 4.如果遇到错误的话，还需要检查输入流的异常屏蔽标志以决定是否抛出相应的异常。
}
}  // namespace n2

namespace n3 {
void func() {
  // 有一种更为有效的途径，STL中最为神秘的法宝之一：istreambuf_iterator。
  // istreambuf_iterator的使用方法与istream_iterator大致相同，
  // 但是istream_iterator<char>对象使用operator>>从输入流中读取单个字符，
  // 而istreambuf_iterator<char>则直接从流的缓存区中读取下一个字符。
  std::string fileData((std::istreambuf_iterator<char>(inputFile)),
                       std::istreambuf_iterator<char>());
  // std::string fileData(std::istreambuf_iterator<char>{inputFile},
  //                       std::istreambuf_iterator<char>{});

  // 不用清除skipws标志，因为istreambuf_iterator不会跳过任何字符，
  // 只是简单地取回流缓冲区中的下一个字符，而不管它们是什么字符。

  // istreambuf_iterator比istream_iterator提高了近40%。
  // 当然，不同的STL实现其速度也会有所不同，istreambuf_iterator还有优化空间。
  // 对于非格式化的逐个字符的输出过程，也应该考虑使用ostreambuf_iterator。
}
}  // namespace n3

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::func();
      break;
    case 2:
      n3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
  return 0;
}