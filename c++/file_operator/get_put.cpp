#include <fstream>
#include <iostream>

// 在某些特殊的场景中，我们可能需要逐个读取文件中存储的字符，或者逐个将字符存储到文件中。
// 这种情况下，就可以调用get()和put()成员方法实现。
// fstream和ofstream类继承自ostream类，因此fstream和ofstream类对象都可以调用put()方法。
// 当fstream和ofstream文件流对象调用put()方法时，该方法的功能就变成了向指定文件中写入单个字符。
// put()方法的语法格式如下：
// ostream& put (char c);
// 其中，c用于指定要写入文件的字符。
// 该方法会返回一个调用该方法的对象的引用形式。
// 例如，obj.put()方法会返回obj这个对象的引用。
// 注意，由于文件存放在硬盘中，硬盘的访问速度远远低于内存。
// 如果每次写一个字节都要访问硬盘，那么文件的读写速度就会慢得不可忍受。
// 因此，操作系统在接收到put()方法写文件的请求时，
// 会先将指定字符存储在一块指定的内存空间中（称为文件流输出缓冲区），
// 等刷新该缓冲区（缓冲区满、关闭文件、手动调用flush()方法等，都会导致缓冲区刷新）时，
// 才会将缓冲区中存储的所有字符“一股脑儿”全写入文件。

// 和put()成员方法的功能相对的是get()方法，其定义在istream类中，借助cin.get()可以读取用户输入的字符。
// 在此基础上，fstream和ifstream类继承自istream类，因此fstream和ifstream类的对象也能调用get()方法。
// 当fstream和ifstream文件流对象调用get()方法时，
// 其功能就变成了从指定文件中读取单个字符（还可以读取指定长度的字符串）。
// 值得一提的是，get()方法的语法格式有很多，这里仅介绍最常用的2种：
// int get();  // 返回值就是读取到的字符，只不过返回的是它的ASCII码，如果碰到输入的末尾，则返回值为EOF。
// istream& get (char& c);  // 需要传递一个字符变量，get()方法会自行将读取到的字符赋值给这个变量。
// 注意，和put()方法一样，操作系统在接收到get()方法的请求后，哪怕只读取一个字符，
// 也会一次性从文件中将很多数据（通常至少是512个字节，
// 因为硬盘的一个扇区是512B）读到一块内存空间中（可称为文件流输入缓冲区），
// 这样当读取下一个字符时，就不需要再访问硬盘中的文件，直接从该缓冲区中读取即可。

int main() {
  char c;
  std::ofstream outFile("out.txt", std::ios::out | std::ios::binary);
  if (!outFile) {
    std::cout << "error" << std::endl;
    return 0;
  }
  while (std::cin >> c) {
    outFile.put(c);
  }
  outFile.close();

  char c1;
  std::ifstream inFile("out.txt", std::ios::out | std::ios::binary);
  if (!inFile) {
    std::cout << "error" << std::endl;
    return 0;
  }
  // while(inFile.get(c))
  while ((c1 = inFile.get()) && c1 != EOF) {
    std::cout << c1;
  }
  inFile.close();

  return 0;
}