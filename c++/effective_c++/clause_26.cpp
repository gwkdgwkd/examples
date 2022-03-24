#include <iostream>
#include <string>

using namespace std;

// 尽可能延后变量定义式出现的时间

string encryptPassword(const string &password) {
  string encrypted;  // 过早定义的变量
  if (password.length() < 8) {
    throw logic_error("Password is too short");  // 此时encrypted没被使用
  }
  // ...  // 必要动作，处理encrypted
  return encrypted;
}

// 延后定义
string encryptPassword1(const string &password) {
  if (password.length() < 8) {
    throw logic_error("Password is too short");
  }
  string encrypted;  // 抛出异常时不会浪费构造和析构成本
  // ...  // 必要动作，处理encrypted
  return encrypted;
}
// encrypted虽然定义却无任何实参作为初值。这意味着调用的是default构造函数。
// 条款4说“通过default构造函数构造初对象然后对它赋值”比“直接在构造时指定初值”效率差。

string encryptPassword2(const string &password) {
  if (password.length() < 8) {
    throw logic_error("Password is too short");
  }
  string encrypted;
  encrypted = password;
  // ...  // 必要动作，处理encrypted
  return encrypted;
}

// 定义并初始化的最佳做法
string encryptPassword3(const string &password) {
  if (password.length() < 8) {
    throw logic_error("Password is too short");
  }
  string encrypted(password);
  // ...  // 必要动作，处理encrypted
  return encrypted;
}

// 不止应该延后变量定义，直到非得使用该变量的前一刻为止，甚至应该尝试延后这份定义直到能够给它初值实参为止。
// 如果这样，不仅能够避免构造和析构非必要对象，还可以避免无意义的default构造行为。

// 如果变量只在循环内使用，那么把它定义于循环外并在每次循环迭代时赋值给它比较好，还是该把它定义于循环内？
// Widget w;
// for(int i  = 0;i < 5; ++i) { // 定义于循环外
//   成本：1个构造函数+1个析构函数+n个赋值操作
//   w=i to Widget
// }
// for(int i  = 0;i < 5; ++i) { // 定义于循环内
//   成本：n个构造函数+n个析构函数
//   Widget w=i to Widget
// }
// 如果class的一个赋值成本低于一组构造+析构成本，定义在循环外比较高效，特别当n值很大的时候。否则定义在循环体内或许较好。
// 此外定义在循环体外，造成w的作用域比定义在循环内大，有时候对程序的可理解性和易维护性造成冲突。

// 因此除非：
//  你知道赋值成本比“构造+析构”成本低;
//  正在处理代码中效率高敏感的部分。
// 否则应该定义在循环体内。

// 请记住：
// 尽可能延后变量定义式的出现。这样做可增加程序的清晰度并改善程序效率。

int main() { return 0; }