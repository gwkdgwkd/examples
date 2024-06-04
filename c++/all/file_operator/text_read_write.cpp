#include <fstream>
#include <iostream>

// 对文件的读/写操作可以细分为2类，分别是以文本形式和以二进制形式读写文件。
// 1.文件中存储的数据并没有类型上的分别，统统都是字符。
//   以文本形式读/写文件，就是直白地将文件中存储的字符（或字符串）读出来，
//   以及将目标字符（或字符串）存储在文件中。
// 2.而以二进制形式读/写文件，操作的对象不再是打开文件就能看到的字符，
//   而是文件底层存储的二进制数据。
//   更详细地讲，当以该形式读取文件时，读取的是该文件底层存储的二进制数据；
//   同样，当将某数据以二进制形式写入到文件中时，写入的也是其对应的二进制数据。
// 以文本形式将浮点数19.625写入文件，则该文件会直接将19.625这个字符串存储起来。
// 当双击打开此文件，也可以看到19.625，但如果以二进制形式将浮点数19.625写入文件，
// 则存储的不再是19.625这个字符串，而是19.625浮点数对应的二进制数据。
// 以float类型的19.625来说，文件最终存储：0100 0001 1001 1101 0000 0000 0000 0000
// 显然，如果直接将以上二进制数据转换为float类型，仍可以得到浮点数19.625。
// 但对于文件来说，它只会将存储的二进制数，
// 即据根据既定的编码格式（如utf-8、gbk等）转换为一个个字符。
// 这也就意味着，如果直接打开此文件，看到的并不会是19.625，往往是一堆乱码。
// C++标准库中，提供了2套读写文件的方法组合，分别是：
// 1.使用>>和<<读写文件：适用于以文本形式读写文件；
// 2.使用read()和write()成员方法读写文件：适用于以二进制形式读写文件。

// fstream或者ifstream类负责实现对文件的读取，它们内部都对>>输出流运算符做了重载；
// fstream和ofstream类负责实现对文件的写入，它们的内部也都对<<输出流运算符做了重载。
// 当fstream或者ifstream类对象打开文件（通常以ios::in作为打开模式）之后，
// 就可以直接借助>>输入流运算符，读取文件中存储的字符（或字符串）；
// 当fstream或者ofstream类对象打开文件（通常以ios::out作为打开模式）后，
// 可以直接借助<<输出流运算符向文件中写入字符（或字符串）。

int main() {
  int x, sum = 0;
  std::ifstream srcFile("in.txt", std::ios::in);  // 以文本模式打开in.txt备读
  if (!srcFile) {                                 // 打开失败
    std::cout << "error opening source file." << std::endl;
    return 0;
  }
  std::ofstream destFile("out.txt",
                         std::ios::out);  // 以文本模式打开out.txt备写
  if (!destFile) {
    srcFile.close();  // 程序结束前不能忘记关闭以前打开过的文件
    std::cout << "error opening destination file." << std::endl;
    return 0;
  }
  // 可以像用cin那样用ifstream对象
  while (srcFile >> x) {
    sum += x;
    // 可以像cout那样使用ofstream对象
    destFile << x << " ";
  }
  std::cout << "sum：" << sum << std::endl;
  destFile.close();
  srcFile.close();

  return 0;
}