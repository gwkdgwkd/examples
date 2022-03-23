#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>

// 通过mismatch或lexicographical_compare实现简单的忽略大小写的字符串比较。

// 只考虑strcmp方法适用的字符串，并不考虑国际化的问题
int ciCharCompare(char c1, char c2) {
  int lc1 = tolower(static_cast<unsigned char>(c1));
  int lc2 = tolower(static_cast<unsigned char>(c2));
  // 如同<cctype>中的很多函数一样，tolower的参数和返回值都是int。
  // 但是，除非该int值是EOF，否则它的值必须可以用unsigned char来表示。
  // 在C和C++中，char可能是有符合的，也可能是无符号的（取决于具体的编译器实现）。
  // 当char有符合时，确保它的值可以用unsigned char来表达的唯一办法是，
  // 在调用tolower之前将它强制转换成一个unsigned char。
  // 这也解释了为什么用int而不是char来保存tolower的返回值

  if (lc1 < lc2) return -1;
  if (lc1 > lc2) return 1;
  return 0;
}

int ciStringCompareImpl(const std::string& s1, const std::string& s2) {
  typedef std::pair<std::string::const_iterator, std::string::const_iterator>
      PSCI;

  // 最繁重的工作由mismatch完成，它返回一对迭代器，指示了这两个区间对应字符第一次比较失败的位置：
  PSCI p = std::mismatch(s1.begin(), s1.end(), s2.begin(),
                         std::not2(std::ptr_fun(ciCharCompare)));
  // not2(ptr_fun(ciCharCompare))这个判别式负责在两个字符匹配时返回true，因为当返回false时mismatch会停下。
  // 不能直接调用ciCharCompare，因为它返回-1、1或者0，而且当两个字符匹配的时候返回0。
  // 如果直接用ciCharCompare作为判别式，那么C++将会把它的返回类型转换成bool。
  // 0->false,-1 1->true，和期望相反。
  if (p.first == s1.end()) {
    // 如果为true，要么s1和s2相等，或者s1比s2短
    if (p.second == s2.end()) {
      return 0;
    } else {
      return -1;
    }
  }
  // 字符串之间的关系和这两个不匹配的字符之间的关系相同
  return ciCharCompare(*p.first, *p.second);
}

int ciStringCompare1(const std::string& s1, const std::string& s2) {
  // 调用mismatch之前，必须先要满足它的前提条件。
  // 特别是，如果两个字符串的长度不一样，那么我们必须把短的字符串作为第一个区间传入。
  if (s1.size() <= s2.size()) {
    return ciStringCompareImpl(s1, s2);
  } else {
    return -ciStringCompareImpl(s2, s1);
  }
}

// 第二种实现方式将产生一个很常用的STL判别式。
// 想这样的函数可以别用作关联容器的比较函数。
// 修改ciCharCompare，使它成为一个具有判别式接口的字符比较函数，
// 然后把执行字符串比较的工作交给STL中名字第二长的算法：
bool ciCharLess(char c1, char c2) {
  return tolower(static_cast<unsigned char>(c1)) <
         tolower(static_cast<unsigned char>(c2));
}
int ciStringCompare2(const std::string& s1, const std::string& s2) {
  // lexicographical_compare名字第二长，最长的是set_symmetric_difference。
  // lexicographical_compare是strcmp的一个泛化版本。
  // strcmp只能与字符数组一起工作，而lexicographical_compare则可以与任何类型的值的区间一起工作。
  // strcmp只是比较两个字符串的关系，而lexicographical_compare则可以接受一个判别式。
  return std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(),
                                      s2.end(), ciCharLess);
}

// C库的非标准扩展中，有忽略大小写的字符串比较函数。具有像strcmp或者strcmpi这样的名字。
// 实现一个忽略大小写的字符串比较函数最容易的方法根本就不需要使用STL。
int ciStringCompare3(const std::string& s1, const std::string& s2) {
  return 0;
  // return stricmp(s1.c_str(), s2.c_str());  // 也许叫别的名字
  // 有人认为这是取巧，但strcmp/strcmpi通常是被优化过的，
  // 它们在长字符串的处理上一般要比通用的算法mismatch和lexicographical_compare快得多。
}

int main() {
  std::string s1 = "nihao";
  std::string s2 = "NiHao";
  std::string s3 = "NiHa";

  std::cout.setf(std::ios::boolalpha);
  std::cout << (ciStringCompare1(s1, s2) == 0) << std::endl;  // true
  std::cout << (ciStringCompare1(s1, s3) == 0) << std::endl;  // false
  std::cout << (ciStringCompare2(s1, s2) == 0) << std::endl;  // true
  std::cout << (ciStringCompare2(s1, s3) == 0) << std::endl;  // true ？

  return 0;
}