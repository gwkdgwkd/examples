#include <fstream>
#include <iostream>

// getline()方法定义在istream类中，而fstream和ifstream类继承自istream类，
// 因此fstream和ifstream的类对象可以调用getline()成员方法。
// 当文件流对象调用getline()方法时，该方法的功能就变成了从指定文件中读取一行字符串。
// 该方法有以下2种语法格式：
//  istream & getline(char* buf, int bufSize);
//  istream & getline(char* buf, int bufSize, char delim);
// 其中，第一种语法格式用于从文件输入流缓冲区中读取bufSize-1个字符到buf，
// 或遇到\n为止（哪个条件先满足就按哪个执行），该方法会自动在buf中读入数据的结尾添加'\0'。
// 第二种语法格式和第一种的区别在于，第一个版本是读到\n为止，第二个版本是读到delim字符为止。
// \n或delim都不会被读入buf，但会被从文件输入流缓冲区中取走。
// 以上2种格式中，getline()方法都会返回一个当前所作用对象的引用。
// 比如，obj.getline()会返回obj的引用。
// 注意，如果文件输入流中\n或delim之前的字符个数达到或超过bufSize，就会导致读取失败。

int main() {
  char c[40];
  std::ifstream inFile("in.txt", std::ios::in | std::ios::binary);
  if (!inFile) {
    std::cout << "error" << std::endl;
    return 0;
  }
  int flag = 0;
  if (flag == 0) {
    inFile.getline(c, 40);  // 1 2 4
    // inFile.getline(c, 40, '4');  // 1 2
    std::cout << c;
    inFile.close();
  } else {
    // 读取文件中的多行数据:
    while (inFile.getline(c, 40)) {
      std::cout << c << std::endl;
    }
    inFile.close();
  }

  return 0;
}