#include <cstring>
#include <fstream>
#include <iostream>

using namespace std;

// 在读写文件时，有时希望直接跳到文件中的某处开始读写，这就需要先将文件的读写指针指向该处，然后再进行读写。
// ifstream类和fstream类有seekg成员函数，可以设置文件读指针的位置；
// ofstream类和fstream类有seekp成员函数，可以设置文件写指针的位置。
// 所谓“位置”，就是指距离文件开头有多少个字节。文件开头的位置是0。
// 这两个函数的原型如下：
//  ostream & seekp (int offset, int mode);
//  istream & seekg (int offset, int mode);
// mode代表文件读写指针的设置模式，有以下三种选项：
//  ios::beg：让文件读指针（或写指针）指向从文件开始向后的offset字节处。offset等于0即代表文件开头。在此情况下，offset只能是非负数。
//  ios::cur：在此情况下，offset为负数则表示将读指针（或写指针）从当前位置朝文件开头方向移动offset字节，为正数则表示将读指针（或写指
//            针）从当前位置朝文件尾部移动offset字节，为0则不移动。
//  ios::end：让文件读指针（或写指针）指向从文件结尾往前的|offset|（offset的绝对值）字节处。在此情况下，offset只能是0或者负数。
// 此外，我们还可以得到当前读写指针的具体位置：
//  ifstream类和fstream类还有tellg成员函数，能够返回文件读指针的位置；
//  ofstream类和fstream类还有tellp成员函数，能够返回文件写指针的位置。
// 这两个成员函数的原型如下：
//  int tellg();
//  int tellp();
// 要获取文件长度，可以用seekg函数将文件读指针定位到文件尾部，再用tellg函数获取文件读指针的位置，此位置即为文件长度。

class CStudent {
 public:
  char szName[20];
  int age;
};

int main() {
  CStudent s;
  fstream ioFile("students.dat", ios::in | ios::out);
  if (!ioFile) {
    cout << "error";
    return 0;
  }
  // 定位读指针到文件尾部，以便用以后tellg获取文件长度
  ioFile.seekg(0, ios::end);
  // L是折半查找范围内第一个记录的序号，R是折半查找范围内最后一个记录的序号
  int L = 0, R;
  // 首次查找范围的最后一个记录的序号就是: 记录总数- 1
  R = ioFile.tellg() / sizeof(CStudent) - 1;
  do {
    int mid = (L + R) / 2;  // 要用查找范围正中的记录和待查找的名字比对
    ioFile.seekg(mid * sizeof(CStudent), ios::beg);  // 定位到正中的记录
    ioFile.read((char *)&s, sizeof(s));
    int tmp = strcmp(s.szName, "Jack");
    if (tmp == 0) {
      s.age = 20;
      ioFile.seekp(mid * sizeof(CStudent), ios::beg);
      ioFile.write((char *)&s, sizeof(s));
      break;
    } else if (tmp > 0)  // 继续到前一半查找
      R = mid - 1;
    else  // 继续到后一半查找
      L = mid + 1;
  } while (L <= R);
  ioFile.close();

  return 0;
}