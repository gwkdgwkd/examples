#include <fstream>
#include <iostream>

// 以二进制形式读写文件有哪些好处？
// 结构体数组要保存到文件中，以文本形式存储不但浪费空间，
// 而且后期不利于查找信息（查找效率低下），因为每条信息所占用的字节数不同。
// 这种情况下，以二进制形式存储到文件中，是非常不错的选择，因为以此形式存储信息，
// 可以直接把对象写入文件中，这意味着每条信息都只占用sizeof(对象)个字节。
// 值得一提的是，要实现以二进制形式读写文件，<<和>>将不再适用，
// 需要使用C++标准库专门提供的read()和write()成员方法。

// ofstream和fstream的write()成员方法实际上继承自ostream类，
// 其功能是将内存中buffer指向的count个字节的内容写入文件，基本格式如下：
// ostream& write(char* buffer, int count);
// 其中，buffer表示要写入文件的二进制数据的起始位置，count用于指定写入字节的个数。
// 该方法可以被ostream类的cout对象调用，常用于向屏幕上输出字符串。
// 同时，它还可以被ofstream或fstream对象调用，将指定个数的二进制数据写入文件。
// 同时，该方法会返回一个作用于该函数的引用形式的对象。
// 需要注意的一点是，write()成员方法向文件中写入若干字节，
// 可是调用write()函数时并没有指定这些字节写入文件中的具体位置。
// 事实上，write()方法会从文件写指针指向的位置将二进制数据写入。
// 所谓文件写指针，是ofstream或fstream对象内部维护的一个变量，文件刚打开时，
// 文件写指针指向的是文件的开头（如果以ios::app方式打开，则指向文件末尾），
// 用write()方法写入n个字节，写指针指向的位置就向后移动n个字节。

// ifstream 和fstream的read()方法实际上继承自istream类，
// 其功能正好和write()方法相反，
// 即从文件中读取count个字节的数据。该方法的语法格式如下：
// istream& read(char* buffer, int count);
// 其中，buffer用于指定读取字节的起始位置，count指定读取字节的个数。
// 该方法也会返回一个调用该方法的对象的引用。
// 和write()方法类似，read()方法从文件读指针指向的位置开始读取若干字节。
// 用read()方法读取n个字节，读指针指向的位置就向后移动n个字节。
// 因此，打开一个文件后连续调用read()方法，就能将整个文件的内容读取出来。
// 另外，在使用read()方法的同时，如果想知道一共成功读取了多少个字节（读到文件尾时，
// 未必能读取count个字节），可以在read()后调用文件流对象的gcount()成员方法，
// 其返回值就是最近一次read()方法成功读取的字节数。

class CStudent {
 public:
  char szName[20];
  int age;
};

int main() {
  CStudent s;
  std::ofstream outFile("students.dat", std::ios::out | std::ios::binary);
  while (std::cin >> s.szName >> s.age) outFile.write((char *)&s, sizeof(s));
  outFile.close();

  CStudent s1;
  // 二进制读方式打开
  std::ifstream inFile("students.dat", std::ios::in | std::ios::binary);
  if (!inFile) {
    std::cout << "error" << std::endl;
    return 0;
  }
  while (inFile.read((char *)&s1, sizeof(s1))) {  // 一直读到文件结束
    std::cout << s.szName << " " << s.age << std::endl;
  }
  inFile.close();

  return 0;
}