#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>

// 通过mismatch或lexicographical_compare实现简单的忽略大小写的字符串比较。

std::string s1 = "nihao";
std::string s2 = "NiHao";
std::string s3 = "NiHa";
std::string s4 = "nihaoa";

namespace n1 {
// 只考虑strcmp方法适用的字符串，并不考虑国际化的问题：
int ciCharCompare(char c1, char c2) {
  int lc1 = tolower(static_cast<unsigned char>(c1));
  int lc2 = tolower(static_cast<unsigned char>(c2));
  // 如同<cctype>中的很多函数一样，tolower的参数和返回值都是int，
  // 但是，除非该int值是EOF，否则它的值必须可以用unsigned char来表示。
  // 在C和C++中，char可能是有符号的，也可能是无符号的（取决编译器）。
  // 当char有符合时，确保它的值可以用unsigned char来表达的唯一办法是，
  // 在调用tolower之前将它强制转换成一个unsigned char。
  // 这也解释了为什么用int而不是char来保存tolower的返回值。

  if (lc1 < lc2) return -1;
  if (lc1 > lc2) return 1;
  return 0;
}

int ciStringCompareImpl(const std::string& s1, const std::string& s2) {
  typedef std::pair<std::string::const_iterator, std::string::const_iterator>
      PSCI;

  // mismatch返回一对迭代器，指示了这两个区间对应字符第一次比较失败的位置：
  PSCI p = std::mismatch(s1.begin(), s1.end(), s2.begin(),
                         std::not2(std::ptr_fun(ciCharCompare)));
  // not2(ptr_fun(ciCharCompare))这个判别式负责在两个字符匹配时返回true，
  // 因为当返回false时mismatch会停下，不能直接调用ciCharCompare，
  // 因为它返回-1、1或者0，而且当两个字符匹配的时候返回0，
  // 如果直接用ciCharCompare作为判别式，那么C++将会把它的返回类型转换成bool，
  // 0->false,-1 1->true，和期望相反。
  if (p.first == s1.end()) {
    if (p.second == s2.end()) {  // s1和s2相等
      return 0;
    } else {  // s1比s2短
      return -1;
    }
  }
  // 字符串之间的关系和这两个不匹配的字符之间的关系相同：
  return ciCharCompare(*p.first, *p.second);
}

int ciStringCompare(const std::string& s1, const std::string& s2) {
  // 如果两个字符串的长度不一样，那么必须把短的字符串作为第一个区间传入：
  if (s1.size() <= s2.size()) {
    return ciStringCompareImpl(s1, s2);
  } else {
    return -ciStringCompareImpl(s2, s1);
  }
}

void func() {
  std::cout << ciStringCompare(s1, s2) << std::endl;  // 0
  std::cout << ciStringCompare(s1, s3) << std::endl;  // 1
  std::cout << ciStringCompare(s2, s4) << std::endl;  // -1
}
}  // namespace n1

namespace n2 {
// 第二种实现方式将产生一个很常用的STL判别式，可以别用作关联容器的比较函数。
// 修改ciCharCompare，使它成为一个具有判别式接口的字符比较函数，
// 然后把执行字符串比较的工作交给STL中名字第二长的算法：
bool ciCharLess(char c1, char c2) {
  return tolower(static_cast<unsigned char>(c1)) <
         tolower(static_cast<unsigned char>(c2));
}
int ciStringCompare(const std::string& s1, const std::string& s2) {
  // lexicographical_compare名字第二长，最长的是set_symmetric_difference。
  // lexicographical_compare是strcmp的一个泛化版本。
  // strcmp只能用于字符数组，lexicographical_compare能用于任何类型的值区间。
  // strcmp只是比较两个字符串的关系，而lexicographical_compare可接受判别式。
  return std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(),
                                      s2.end(), ciCharLess);
}

void func() {
  std::cout << ciStringCompare(s1, s2) << std::endl;  // 0
  std::cout << ciStringCompare(s1, s3) << std::endl;  // 1
  std::cout << ciStringCompare(s2, s4) << std::endl;  // -1
}
}  // namespace n2

namespace n3 {
// C库的非标准扩展中，有忽略大小写的字符串比较函数，像strcmp或者strcmpi。
// 实现一个忽略大小写的字符串比较函数最容易的方法根本就不需要使用STL。
int ciStringCompare(const std::string& s1, const std::string& s2) {
  return 0;
  // return stricmp(s1.c_str(), s2.c_str());  // 也许叫别的名字
  // 有人认为这是取巧，但strcmp/strcmpi通常是被优化过的，特别是对长字符串，
  // 一般要比通用的算法mismatch和lexicographical_compare快得多。
}
}  // namespace n3

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
  return 0;
}